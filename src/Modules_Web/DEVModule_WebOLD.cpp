#include "DEVModule_Web.h"
#include "HempyModule_Web.h"
#include "AeroModule_Web.h"
#include "ReservoirModule_Web.h"
#include "../Components/WaterPump.h"
#include "../Components_Web/DHTSensor_Web.h"
#include "../Components_Web/DistanceSensor_Web.h"
#include "../Components_Web/Lights_Web.h"
#include "../Components_Web/LightSensor_Web.h"
#include "../Components_Web/Sound_Web.h"
#include "../Components_Web/Fan_Web.h"
#include "../Components_Web/PowerSensor_Web.h"
#include "../Components_Web/PowerSensorV3_Web.h" ///< Only for PZEM004T V3.0
#include "../Components_Web/PHSensor_Web.h"
#include "../Components_Web/PressureSensor_Web.h"
#include "../Components_Web/Aeroponics_Tank_Web.h"
#include "../Components_Web/Aeroponics_NoTank_Web.h"
#include "../Components_Web/WaterLevelSensor_Web.h"
#include "../Components_Web/WaterTempSensor_Web.h"
//#include "../Components_Web/WeightSensor_Web.h"
#include "../Components_Web/ModuleSkeleton_Web.h" ///< Only for demonstration purposes

DEVModule_Web::DEVModule_Web(const __FlashStringHelper *Name, Settings::MainModuleSettings *DefaultSettings, RF24 *Wireless) : Common(Name), Common_Web(Name), Module_Web(Wireless)
{ ///< Constructor
  this->Name = Name;
  SheetsReportingFrequency = &DefaultSettings->SheetsReportingFrequency;
  ReportToGoogleSheets = &DefaultSettings->ReportToGoogleSheets;
  Sound1 = new Sound_Web(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  ///< ModuleSkeleton1 = new ModuleSkeleton_Web(F("ModuleSkeleton1"),this,&ModuleSettings -> ModuleSkeleton1);  ///< Only for demonstration purposes
  ///< ModuleSkeleton2 = new ModuleSkeleton_Web(F("ModuleSkeleton2"),this,&ModuleSettings -> ModuleSkeleton2);  ///< Only for demonstration purposes
  IFan = new Fan_Web(F("IFan"), this, &ModuleSettings->IFan); ///< passing: Component name, DEVModule_Web object the component belongs to, Default settings)
  EFan = new Fan_Web(F("EFan"), this, &ModuleSettings->EFan);
  Lt1 = new Lights_Web(F("Lt1"), this, &ModuleSettings->Lt1);
  LtSen1 = new LightSensor_Web(F("LtSen1"), this, &ModuleSettings->LtSen1, Lt1); ///< Passing an extra Light object as parameter: Calibrates the light sensor against the passed Light object
  Pow1 = new PowerSensor_Web(F("Pow1"), this, &Serial2);
  ///< Pow1 = new PowerSensorV3_Web(F("Pow1"), this, &Serial2); ///< Only for PZEM004T V3.0
  DHT1 = new DHTSensor_Web(F("DHT1"), this, &ModuleSettings->DHT1);
  //PHSen1 = new PHSensor_Web(F("PHSen1"), this, &ModuleSettings->PHSen1);
  //WTemp1 = new WaterTempSensor_Web(F("WTemp1"), this, &ModuleSettings->WTemp1);
  //WLev1 = new WaterLevelSensor_Web(F("WLev1"), this, &ModuleSettings->WLev1);
  //Dist1 = new DistanceSensor_Web(F("Dist1"), this, &ModuleSettings->Dist1);
  HempyModule1 = new HempyModule_Web(F("Hemp1"), this, &ModuleSettings->HempyModule1);
  //Weight1 = new WeightSensor_Web(F("Weight1"), this, &ModuleSettings->Weight1);
  //Weight2 = new WeightSensor_Web(F("Weight2"), this, &ModuleSettings->Weight2);
  AeroModule1 = new AeroModule_Web(F("Aero1"), this, &ModuleSettings->AeroModule1);
  ReservoirModule1 = new ReservoirModule_Web(F("Res1"), this, &ModuleSettings->ReservoirMod1);
  //Pres1 = new PressureSensor_Web(F("Pres1"), this, &ModuleSettings->Pres1);
  //AeroPump1 = new WaterPump(F("AeroPump1"),this,&ModuleSettings->AeroPump1);
  //AeroT1 = new Aeroponics_Tank_Web(F("AeroT1"), this, &ModuleSettings->AeroT1, Pres1, AeroPump1); ///< Passing the pressure sensor object that monitors the pressure inside the Aeroponics system
  //AeroNT1 = new Aeroponics_NoTank_Web(F("AeroNT1"), this, &ModuleSettings->AeroNT1, Pres1, AeroPump1);

  addToRefreshQueue_FiveSec(this);
  addToRefreshQueue_Minute(this);
  addToWebsiteQueue_Load(this);
  addToWebsiteQueue_Refresh(this);
  addToWebsiteQueue_Field(this);
  Parent->addToCommandQueue(this);
  logToSerials(Name, false, 0);
  logToSerials(F("refreshing"), true, 1);
  runAll();
  addToLog(F("DEVModule_Web initialized"), 0);
}

void DEVModule_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end

  strcat_P(LongMessage, (PGM_P)F("\"Metric\":\""));
  strcat(LongMessage, toText(Metric));
  strcat_P(LongMessage, (PGM_P)F("\",\"Debug\":\""));
  strcat(LongMessage, toText(Debug));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void DEVModule_Web::websiteEvent_Load(char *Url)
{
  ;
}

void DEVModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url) ///< called when website is refreshed.
{
  WebServer.setArgString(F("Time"), getFormattedTime(false));
  WebServer.setArgJson(F("Log"), eventLogToJSON(false, true)); ///< Last events that happened in JSON format
}

void DEVModule_Web::commandEvent(char *Button)
{ ///< When a button is pressed on the website
  if (!isThisMine(Button))
  {
    return false;
  }
  else
  {
    return true;
  }
}

void DEVModule_Web::refresh_FiveSec()
{
  Module_Web::refresh_FiveSec();
}

void DEVModule_Web::refresh_Minute()
{
  Module_Web::refresh_Minute()
}