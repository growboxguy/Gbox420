#include "MainModule_Web.h"
#include "HempyModule_Web.h"
#include "AeroModule_Web.h"
#include "ReservoirModule_Web.h"
#include "ACMotorModule_Web.h"
#include "../Components_Web/Sound_Web.h"
#include "../Components_Web/DHTSensor_Web.h"
#include "../Components_Web/Lights_Web.h"
#include "../Components_Web/LightSensor_Web.h"
#include "../Components_Web/Fan_Web.h"
//#include "../Components_Web/Fan_PWM_Web.h"
#include "../Components_Web/AirPump_Web.h"
//#include "../Components_Web/PowerSensor_Web.h"  ///< For PZEM004T V1.0 or PZEM004T V2.0
#include "../Components_Web/PowerSensorV3_Web.h" ///< Only for PZEM004T V3.0

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
*/
MainModule::MainModule(const __FlashStringHelper *Name, Settings::MainModuleSettings *DefaultSettings, RF24 *Wireless) : Common(Name), Common_Web(Name), Module(Name), Module_Web(Name)
{
  SerialReportFrequency = &DefaultSettings->SerialReportFrequency;
  SerialReportDate = &DefaultSettings->SerialReportDate;
  SerialReportMemory = &DefaultSettings->SerialReportMemory;
  SerialReportJSONFriendly = &DefaultSettings->SerialReportJSONFriendly;
  SerialReportJSON = &DefaultSettings->SerialReportJSON;
  SerialReportWireless = &DefaultSettings->SerialReportWireless;
  ReportToGoogleSheets = &DefaultSettings->ReportToGoogleSheets;
  //ReportToHomeAssistant = &DefaultSettings->ReportToHomeAssistant;
  SheetsReportingFrequency = &DefaultSettings->SheetsReportingFrequency;
  ReportToMqtt = &DefaultSettings->ReportToMqtt;
  MQTTReportFrequency = &DefaultSettings->MQTTReportFrequency;

  logToSerials(F(""), true, 0); //<Line break
  this->Wireless = Wireless;
  Sound1 = new Sound_Web(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;                                       ///< Pointer for child objects to use sound feedback
  FanI = new Fan_Web(F("FanI"), this, &ModuleSettings->FanI);         ///< Internal fan. passing parameters: 1. Component name; 2. MainModule object the component belongs to; 3. Persistent settings stored in EEPROM)
  FanE = new Fan_Web(F("FanE"), this, &ModuleSettings->FanE);         ///< Exhaust fan.
  //FanI = new Fan_PWM(F("FanI"), this, &ModuleSettings->FanI);
  //FanE = new Fan_PWM(F("FanE"), this, &ModuleSettings->FanE);
  Ap1 = new AirPump_Web(F("Ap1"), this, &ModuleSettings->Ap1);          ///< Air pump
  Lt1 = new Lights_Web(F("Lt1"), this, &ModuleSettings->Lt1);           ///< Light1
  Lt2 = new Lights_Web(F("Lt2"), this, &ModuleSettings->Lt2);           ///< Light2 (optional)
  Ls1 = new LightSensor_Web(F("Ls1"), this, &ModuleSettings->Ls1, Lt1); ///< Passing an extra Light object as parameter: Calibrates the light sensor against the passed Light object
  DHT1 = new DHTSensor_Web(F("DHT1"), this, &ModuleSettings->DHT1);
  //Pow1 = new PowerSensor_Web(F("Pow1"), this, &Serial2); ///< For PZEM004T V1.0 or PZEM004T V2.0
  Pow1 = new PowerSensorV3_Web(F("Pow1"), this, &Serial2);                                     ///< Only for PZEM004T V3.0
  HempyModule1 = new HempyModule_Web(F("Hemp1"), this, &ModuleSettings->HempyModule1);         ///< Module used to relay Settings/MQTT/Website commands to the Hempy module and receive sensor readings
  AeroModule1 = new AeroModule_Web(F("Aero1"), this, &ModuleSettings->AeroModule1);            ///< Module used to relay Settings/MQTT/Website commands to the Aeroponics module and receive sensor readings
  ReservoirModule1 = new ReservoirModule_Web(F("Res1"), this, &ModuleSettings->ReservoirMod1); ///< Module used to relay Settings/MQTT/Website commands to the Reservoir module and receive sensor readings
  //ACMotorModule1 = new ACMotorModule_Web(F("Moto1"), this, &ModuleSettings->ACMotor1);          ///< Module used to relay Settings/MQTT/Website commands to the Reservoir module and receive sensor readings
  addToReportQueue(this);                                                                      //< Attach to the report event: When triggered the module reports to the Serial Console or to MQTT
  addToRefreshQueue_FiveSec(this);                                                             //< Attach to a trigger that fires every five seconds and calls refresh_FiveSec()
  addToRefreshQueue_Minute(this);                                                              //< Attach to a trigger that fires every second and calls refresh_Sec()
  addToWebsiteQueue_Load(this);                                                                //< Attach to the ESP-link website load event: Calls websiteEvent_Load() when an ESP-link webpage is opened
  addToWebsiteQueue_Refresh(this);                                                             //< Attach to the ESP-link website refresh event: Calls websiteEvent_Refresh() when an ESP-link webpage is refreshing
  addToCommandQueue(this);
  addToLog(F("MainModule initialized"), 0);
  logToSerials(Name, false, 0);
  logToSerials(F("refreshing"), true, 1);
  runAll();
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void MainModule::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"M\":\""));
  strcat(LongMessage, getMetricText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, getDebugText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void MainModule::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  ;
}

void MainModule::websiteEvent_Refresh(__attribute__((unused)) char *Url) ///< called when website is refreshed.
{
  WebServer.setArgString(F("Time"), getFormattedTime(false));
  WebServer.setArgJson(F("Log"), eventLogToJSON(false, true)); ///< Last events that happened in JSON format
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
bool MainModule::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    return true;
  }
}

void MainModule::refresh_FiveSec()
{
  Module_Web::refresh_FiveSec();
}

void MainModule::refresh_Minute()
{
  Module_Web::refresh_Minute();
}

bool MainModule::getDayMode()
{
  if (Lt1->getStatus() || Lt2->getStatus() || !(Ls1->getDark()))
  {
    return true; ///< Return true if any of the lights are on OR the light sensor is detecting light
  }
  else
  {
    return false;
  }
}

char *MainModule::getDayModeText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_onOff(getDayMode());
  }
  else
  {
    return toText(getDayMode());
  }
}