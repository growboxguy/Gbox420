#include "MainModule_Web.h"
#include "HempyModule_Web.h"
#include "AeroModule_Web.h"
#include "ReservoirModule_Web.h"
#include "../Components_Web/Sound_Web.h"
#include "../Components/DHTSensor.h"
#include "../Components/Lights.h"
#include "../Components/LightSensor.h"
#include "../Components/Fan.h"
#include "../Components/Fan_PWM.h"
#include "../Components/AirPump.h"
//#include "../Components/PowerSensor.h"  ///< For PZEM004T V1.0 or PZEM004T V2.0
#include "../Components/PowerSensorV3.h" ///< Only for PZEM004T V3.0

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
*/
MainModule::MainModule(const __FlashStringHelper *Name, Settings::MainModuleSettings *DefaultSettings, RF24 *Wireless) : Common_Web(Name), Common(Name), Module_Web(Name), Module(Name)
{
  SerialReportFrequency = &DefaultSettings->SerialReportFrequency;
  SerialReportDate = &DefaultSettings->SerialReportDate;
  SerialReportMemory = &DefaultSettings->SerialReportMemory;
  SerialReportJSONFriendly = &DefaultSettings->SerialReportJSONFriendly;
  SerialReportJSON = &DefaultSettings->SerialReportJSON;
  SerialReportWireless = &DefaultSettings->SerialReportWireless;
  ReportToGoogleSheets = &DefaultSettings->ReportToGoogleSheets;
  SheetsReportingFrequency = &DefaultSettings->SheetsReportingFrequency;
  ReportToMQTT = &DefaultSettings->ReportToMQTT;
  MQTTReportFrequency = &DefaultSettings->MQTTReportFrequency;

  logToSerials(F(""), true, 0); //<Line break
  this->Wireless = Wireless;
  Sound1 = new Sound_Web(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;                                   ///< Pointer for child objects to use sound feedback
  IFan = new Fan(F("IFan"), this, &ModuleSettings->IFan);         ///< passing parameters: 1. Component name; 2. MainModule object the component belongs to; 3. Persistent settings stored in EEPROM)
  EFan = new Fan(F("EFan"), this, &ModuleSettings->EFan);
  //FanI = new Fan_PWM(F("FanI"), this, &ModuleSettings->FanI);
  //FanE = new Fan_PWM(F("FanE"), this, &ModuleSettings->FanE);
  APump1 = new AirPump(F("APump1"), this, &ModuleSettings->APump1);
  Lt1 = new Lights(F("Lt1"), this, &ModuleSettings->Lt1);
  Lt2 = new Lights(F("Lt2"), this, &ModuleSettings->Lt2);
  LtSen1 = new LightSensor(F("LtSen1"), this, &ModuleSettings->LtSen1, Lt1); ///< Passing an extra Light object as parameter: Calibrates the light sensor against the passed Light object
  DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  //Pow1 = new PowerSensor(F("Pow1"), this, &Serial2); ///< For PZEM004T V1.0 or PZEM004T V2.0
  Pow1 = new PowerSensorV3(F("Pow1"), this, &Serial2);                                         ///< Only for PZEM004T V3.0
  HempyModule1 = new HempyModule_Web(F("Hemp1"), this, &ModuleSettings->HempyModule1);         ///< Module used to relay Settings/MQTT/Website commands to the Hempy module and receive sensor readings
  AeroModule1 = new AeroModule_Web(F("Aero1"), this, &ModuleSettings->AeroModule1);            ///< Module used to relay Settings/MQTT/Website commands to the Aeroponics module and receive sensor readings
  ReservoirModule1 = new ReservoirModule_Web(F("Res1"), this, &ModuleSettings->ReservoirMod1); ///< Module used to relay Settings/MQTT/Website commands to the Reservoir module and receive sensor readings
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

void MainModule::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0) //GrowBox tab
  {
    //WebServer.setArgInt(getName(F("FIS")), FanI->getSpeed()); ///< Internal PWM Fan speed
    //WebServer.setArgInt(getName(F("FES")), FanE->getSpeed()); ///< Exhaust PWM Fan speed
    //Light1
    WebServer.setArgInt(getName(F("L1OnH")), *(Lt1->OnHour));    ///< On hour
    WebServer.setArgInt(getName(F("L1OnM")), *(Lt1->OnMinute));  ///< On minute
    WebServer.setArgInt(getName(F("L1OfH")), *(Lt1->OffHour));   ///< Off hour
    WebServer.setArgInt(getName(F("L1OfM")), *(Lt1->OffMinute)); ///< Off minute
    WebServer.setArgInt(getName(F("L1B")), *(Lt1->Brightness));  ///< Brightness percentage
    //Light2
    WebServer.setArgInt(getName(F("L2OnH")), *(Lt2->OnHour));    ///< On hour
    WebServer.setArgInt(getName(F("L2OnM")), *(Lt2->OnMinute));  ///< On minute
    WebServer.setArgInt(getName(F("L2OfH")), *(Lt2->OffHour));   ///< Off hour
    WebServer.setArgInt(getName(F("L2OfM")), *(Lt2->OffMinute)); ///< Off minute
    WebServer.setArgInt(getName(F("L2B")), *(Lt2->Brightness));  ///< Brightness percentage
  }
  else if (strncmp(url, "/S", 2) == 0) //Settings tab
  {
    WebServer.setArgInt(getName(F("Debug")), *Debug);
    WebServer.setArgInt(getName(F("Metric")), *Metric);
    WebServer.setArgInt(getName(F("SerialF")), *SerialReportFrequency);
    WebServer.setArgInt(getName(F("Date")), *SerialReportDate);
    WebServer.setArgInt(getName(F("Mem")), *SerialReportMemory);
    WebServer.setArgInt(getName(F("JSON")), *SerialReportJSON);
    WebServer.setArgInt(getName(F("FJSON")), *SerialReportJSONFriendly);
    WebServer.setArgInt(getName(F("Wire")), *SerialReportWireless);
    WebServer.setArgBoolean(getName(F("Sheets")), *ReportToGoogleSheets);
    WebServer.setArgInt(getName(F("SheetsF")), *SheetsReportingFrequency);
    WebServer.setArgString(getName(F("Relay")), ModuleSettings->PushingBoxLogRelayID);
    WebServer.setArgBoolean(getName(F("MQTT")), *ReportToMQTT);
    WebServer.setArgInt(getName(F("MQTTF")), *MQTTReportFrequency);
    WebServer.setArgString(getName(F("MPT")), ModuleSettings->MqttPubTopic);
    WebServer.setArgString(getName(F("MST")), ModuleSettings->MqttSubTopic);
    WebServer.setArgString(getName(F("MLT")), ModuleSettings->MqttLwtTopic);
    WebServer.setArgString(getName(F("MLM")), ModuleSettings->MqttLwtMessage);
  }
}

void MainModule::websiteEvent_Refresh(__attribute__((unused)) char *url) ///< called when website is refreshed.
{
  //All tabs
  WebServer.setArgString(getName(F("Time")), getFormattedTime(false));
  WebServer.setArgJson(getName(F("Log")), eventLogToJSON(false, true)); ///< Last events that happened in JSON format

  if (strncmp(url, "/G", 2) == 0) //GrowBox tab
  {
    //Air pump
    WebServer.setArgString(getName(F("AP")), APump1->getStateText(true));
    //DHT1
    WebServer.setArgString(getName(F("DT")), DHT1->getTempText(true)); ///< Shows the latest reading
    WebServer.setArgString(getName(F("DH")), DHT1->getHumidityText(true));
    //PWM FAN
    //WebServer.setArgString(getName(F("FIST")), getSpeedText(true,true));  ///PWM Fan speed text
    //WebServer.setArgString(getName(F("FEST")), getSpeedText(true,true));  ///PWM Fan speed text
    //Internal Fan
    WebServer.setArgString(getName(F("IFS")), IFan->fanSpeedText(true));
    //Exhaust Fan
    WebServer.setArgString(getName(F("EFS")), EFan->fanSpeedText(true));
    //Light1
    WebServer.setArgString(getName(F("L1S")), Lt1->getStateText());                  ///< State
    WebServer.setArgString(getName(F("L1Br")), Lt1->getCurrentBrightnessText(true)); ///< Timer on or off
    WebServer.setArgString(getName(F("L1T")), Lt1->getTimerOnOffText(true));         ///< Timer on or off
    //Light2
    WebServer.setArgString(getName(F("L2S")), Lt2->getStateText());                  ///< State
    WebServer.setArgString(getName(F("L2Br")), Lt2->getCurrentBrightnessText(true)); ///< Timer on or off
    WebServer.setArgString(getName(F("L2T")), Lt2->getTimerOnOffText(true));         ///< Timer on or off
    //LightSensor1
    WebServer.setArgString(getName(F("LSD")), LtSen1->getDarkText(true));
    WebServer.setArgString(getName(F("LSR")), LtSen1->getReadingText(true));
    //PowerSensor
    WebServer.setArgString(getName(F("PP")), Pow1->getPowerText(true));
    WebServer.setArgString(getName(F("PE")), Pow1->getEnergyText(true));
    WebServer.setArgString(getName(F("PV")), Pow1->getVoltageText(true));
    WebServer.setArgString(getName(F("PC")), Pow1->getCurrentText(true));
    //PowerSensor V3
    //WebServer.setArgString(getName(F("PF")), Pow1 -> getFrequencyText(true));
    //WebServer.setArgString(getName(F("PPF")), Pow1 -> getPowerFactorText());
  }
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
bool MainModule::commandEvent(char *Command, char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    //Air pump
    if (strcmp_P(ShortMessage, (PGM_P)F("AP")) == 0)
    {
      APump1->setState(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("APOn")) == 0)
    {
      APump1->setState(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("APOff")) == 0)
    {
      APump1->setState(false);
    }
    //Internal Fan
    else if (strcmp_P(ShortMessage, (PGM_P)F("IFO")) == 0)
    {
      IFan->TurnOff();
      WebServer.setArgString(getName(F("IFS")), IFan->fanSpeedText(true));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("IFL")) == 0)
    {
      IFan->SetLowSpeed();
      WebServer.setArgString(getName(F("IFS")), IFan->fanSpeedText(true));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("IFH")) == 0)
    {
      IFan->SetHighSpeed();
      WebServer.setArgString(getName(F("IFS")), IFan->fanSpeedText(true));
    }
    //Exhaust Fan
    else if (strcmp_P(ShortMessage, (PGM_P)F("EFO")) == 0)
    {
      EFan->TurnOff();
      WebServer.setArgString(getName(F("EFS")), EFan->fanSpeedText(true));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("EFL")) == 0)
    {
      EFan->SetLowSpeed();
      WebServer.setArgString(getName(F("EFS")), EFan->fanSpeedText(true));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("EFH")) == 0)
    {
      EFan->SetHighSpeed();
      WebServer.setArgString(getName(F("EFS")), EFan->fanSpeedText(true));
    }
    //Light1
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1")) == 0)
    {
      Lt1->setLightOnOff(toBool(Data), true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1On")) == 0)
    {
      Lt1->setLightOnOff(true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1Of")) == 0)
    {
      Lt1->setLightOnOff(false, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1T")) == 0)
    {
      Lt1->setTimerOnOff(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1TOn")) == 0)
    {
      Lt1->setTimerOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1TOff")) == 0)
    {
      Lt1->setTimerOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1D")) == 0)
    {
      Lt1->dimLightsOnOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1B")) == 0)
    {
      Lt1->setBrightness(toInt(Data), false, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OnT")) == 0)
    {
      Lt1->setOnTime(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OnH")) == 0)
    {
      Lt1->setOnHour(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OnM")) == 0)
    {
      Lt1->setOnMinute(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OfT")) == 0)
    {
      Lt1->setOffTime(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OfH")) == 0)
    {
      Lt1->setOffHour(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OfM")) == 0)
    {
      Lt1->setOffMinute(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1DD")) == 0)
    {
      Lt1->setDimDuration(toInt(Data));
    }
    //Light2
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2")) == 0)
    {
      Lt2->setLightOnOff(toBool(Data), true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2On")) == 0)
    {
      Lt2->setLightOnOff(true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2Of")) == 0)
    {
      Lt2->setLightOnOff(false, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2T")) == 0)
    {
      Lt2->setTimerOnOff(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2TOn")) == 0)
    {
      Lt2->setTimerOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2TOff")) == 0)
    {
      Lt2->setTimerOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2D")) == 0)
    {
      Lt2->dimLightsOnOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2B")) == 0)
    {
      Lt2->setBrightness(toInt(Data), true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OnT")) == 0)
    {
      Lt2->setOnTime(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OnH")) == 0)
    {
      Lt2->setOnHour(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OnM")) == 0)
    {
      Lt2->setOnMinute(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OfT")) == 0)
    {
      Lt2->setOffTime(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OfH")) == 0)
    {
      Lt2->setOffHour(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OfM")) == 0)
    {
      Lt2->setOffMinute(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2DD")) == 0)
    {
      Lt2->setDimDuration(toInt(Data));
    }
    else if (Module_Web::commandEvent(Command, Data))
    {
    }
    else
    {
      return false;
    }
    return true;
  }
}

void MainModule::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  reportToSerialTrigger();
  reportToMQTTTrigger();
  if (RefreshAllRequested)
  {
    RefreshAllRequested = false;
    runAll();
  }
  if (ReportToGoogleSheetsRequested)
  {
    ReportToGoogleSheetsRequested = false;
    reportToGoogleSheetsTrigger(true);
  }
  if (ConsoleReportRequested)
  {
    ConsoleReportRequested = false;
    runReport();
  }
  if (MQTTReportRequested)
  {
    MQTTReportRequested = false;
    reportToMQTTTrigger(true);
  }
}

void MainModule::refresh_Minute()
{
  Common::refresh_Minute();
  reportToGoogleSheetsTrigger();
}

bool MainModule::getDayMode()
{
  if (Lt1->getStatus() || Lt2->getStatus() || !(LtSen1->getDark()))
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