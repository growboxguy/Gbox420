#include "MainModule_Web.h"
#include "HempyModule_Web.h"
#include "AeroModule_Web.h"
#include "ReservoirModule_Web.h"
#include "../Components_Web/DHTSensor_Web.h"
#include "../Components_Web/Lights_Web.h"
#include "../Components_Web/LightSensor_Web.h"
#include "../Components_Web/Sound_Web.h"
#include "../Components_Web/Fan_Web.h"
#include "../Components_Web/Fan_PWM_Web.h"
#include "../Components_Web/AirPump_Web.h"
//#include "../Components_Web/PowerSensor_Web.h"  ///< For PZEM004T V1.0 or PZEM004T V2.0
#include "../Components_Web/PowerSensorV3_Web.h" ///< Only for PZEM004T V3.0

MainModule::MainModule(const __FlashStringHelper *Name, Settings::MainModuleSettings *DefaultSettings, RF24 *Wireless) : Common(Name), Common_Web(Name), Module_Web(Wireless)
{ ///< Constructor
  SheetsReportingFrequency = &DefaultSettings->SheetsReportingFrequency;
  ReportToGoogleSheets = &DefaultSettings->ReportToGoogleSheets;
  Sound1 = new Sound_Web(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  IFan = new Fan_Web(F("IFan"), this, &ModuleSettings->IFan); ///< passing: Component name, MainModule object the component belongs to, Default settings)
  EFan = new Fan_Web(F("EFan"), this, &ModuleSettings->EFan);
  FanI = new Fan_PWM_Web(F("FanI"), this, &ModuleSettings->FanI);
  FanE = new Fan_PWM_Web(F("FanE"), this, &ModuleSettings->FanE);
  APump1 = new AirPump_Web(F("APump1"), this, &ModuleSettings->APump1);
  Lt1 = new Lights_Web(F("Lt1"), this, &ModuleSettings->Lt1);
  Lt2 = new Lights_Web(F("Lt2"), this, &ModuleSettings->Lt2);
  LtSen1 = new LightSensor_Web(F("LtSen1"), this, &ModuleSettings->LtSen1, Lt1); ///< Passing an extra Light object as parameter: Calibrates the light sensor against the passed Light object
  DHT1 = new DHTSensor_Web(F("DHT1"), this, &ModuleSettings->DHT1);
  //Pow1 = new PowerSensor_Web(F("Pow1"), this, &Serial2); ///< For PZEM004T V1.0 or PZEM004T V2.0
  Pow1 = new PowerSensorV3_Web(F("Pow1"), this, &Serial2); ///< Only for PZEM004T V3.0
  HempyModule1 = new HempyModule_Web(F("Hemp1"), this, &ModuleSettings->HempyModule1);
  AeroModule1 = new AeroModule_Web(F("Aero1"), this, &ModuleSettings->AeroModule1);
  ReservoirModule1 = new ReservoirModule_Web(F("Res1"), this, &ModuleSettings->ReservoirMod1);
  addToReportQueue(this);
  addToRefreshQueue_FiveSec(this);
  addToRefreshQueue_Minute(this);
  addToRefreshQueue_QuarterHour(this);
  addToWebsiteQueue_Load(this);
  addToWebsiteQueue_Refresh(this);
  addToWebsiteQueue_Field(this);
  addToWebsiteQueue_Button(this);
  logToSerials(F("MainModule object created, refreshing..."), true, 0);
  runAll();
  addToLog(F("MainModule initialized"), 0);
}

void MainModule::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("Debug:"));
  strcat(LongMessage, toText_enabledDisabled(Debug));
  strcat_P(LongMessage, (PGM_P)F(" ; Metric mode:"));
  strcat(LongMessage, toText_enabledDisabled(Metric));
  logToSerials(&LongMessage, true, 1);
}

void MainModule::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end

  strcat_P(LongMessage, (PGM_P)F("\"Metric\":\""));
  strcat(LongMessage, toText(*Metric));
  strcat_P(LongMessage, (PGM_P)F("\",\"Debug\":\""));
  strcat(LongMessage, toText(*Debug));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void MainModule::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/S", 2) == 0)
  {
    WebServer.setArgInt(getComponentName(F("Debug")), *Debug);
    WebServer.setArgInt(getComponentName(F("Metric")), *Metric);
    WebServer.setArgBoolean(getComponentName(F("Sheets")), *ReportToGoogleSheets);
    WebServer.setArgInt(getComponentName(F("SheetsF")), *SheetsReportingFrequency);
    WebServer.setArgString(getComponentName(F("Relay")), ModuleSettings->PushingBoxLogRelayID);
  }
}

void MainModule::websiteEvent_Refresh(__attribute__((unused)) char *url) ///< called when website is refreshed.
{
  WebServer.setArgString(F("Time"), getFormattedTime(false));
  WebServer.setArgJson(F("Log"), eventLogToJSON()); ///< Last events that happened in JSON format
}

void MainModule::websiteEvent_Button(char *Button)
{ ///< When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("SheetsRep")) == 0)
    {
      ReportToGoogleSheetsRequested = true; ///< just signal that a report should be sent, do not actually run it: Takes too long from an interrupt
      addToLog(F("Reporting to Sheets"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SerialRep")) == 0)
    {
      ConsoleReportRequested = true;
      addToLog(F("Reporting to Serial"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Refresh")) == 0) ///< Website signals to refresh all sensor readings
    {
      RefreshAllRequested = true;
      addToLog(F("Refresh triggered"), false);
    }
  }
}

void MainModule::websiteEvent_Field(char *Field)
{ ///< When the website field is submitted
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Debug")) == 0)
    {
      setDebug(WebServer.getArgBoolean());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Metric")) == 0)
    {
      setMetric(WebServer.getArgBoolean());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Sheets")) == 0)
    {
      setSheetsReportingOnOff(WebServer.getArgBoolean());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SheetsF")) == 0)
    {
      setSheetsReportingFrequency(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Relay")) == 0)
    {
      setPushingBoxLogRelayID(WebServer.getArgString());
    }
  }
}

void MainModule::refresh_FiveSec()
{
  if (*Debug)
  {
    Common::refresh_FiveSec();
    runReport();
  }
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
}

void MainModule::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  runReport();
}

void MainModule::refresh_QuarterHour()
{
  if (*Debug)
    Common::refresh_QuarterHour();
  reportToGoogleSheetsTrigger();
}

bool MainModule::getDayMode()
{
  if (Lt1->getStatus() || Lt2->getStatus() || !(LtSen1->getDark()))
  {
    return true; ///< Return true if any of the lights are on OR the light sensor is detecting daylight
  }
  else
  {
    return false;
  }
}

///< Settings
void MainModule::setDebug(bool DebugEnabled)
{
  *Debug = DebugEnabled;
  if (*Debug)
  {
    addToLog(F("Debug enabled"));
    getSoundObject()->playOnSound();
  }
  else
  {
    addToLog(F("Debug disabled"));
    getSoundObject()->playOffSound();
  }
}

void MainModule::setMetric(bool MetricEnabled)
{
  if (MetricEnabled != *Metric)
  { ///< if there was a change
    *Metric = MetricEnabled;
    RefreshAllRequested = true;
  }
  if (*Metric)
    addToLog(F("Using Metric units"));
  else
    addToLog(F("Using Imperial units"));
  getSoundObject()->playOnSound();
}

///< Google Sheets reporting

void MainModule::setSheetsReportingOnOff(bool State)
{
  *ReportToGoogleSheets = State;
  if (State)
  {
    addToLog(F("Google Sheets enabled"));
    getSoundObject()->playOnSound();
  }
  else
  {
    addToLog(F("Google Sheets disabled"));
    getSoundObject()->playOffSound();
  }
}

void MainModule::setSheetsReportingFrequency(uint16_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog(F("Reporting freqency updated"));
  getSoundObject()->playOnSound();
}

void MainModule::reportToGoogleSheetsTrigger(bool ForceRun)
{ ///< Handles custom reporting frequency for Google Sheets, called every 15 minutes
  if (SheetsRefreshCounter == 96)
    SheetsRefreshCounter = 0; ///< Reset the counter after one day (15 x 96 = 1440 = 24 hours)
  if (SheetsRefreshCounter++ % (*SheetsReportingFrequency / 15) == 0 || ForceRun)
  {
    addPushingBoxLogRelayID();                    ///< Adds a curly bracket {  that needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"Log\":{")); ///< Adds a curly bracket {  that needs to be closed at the end
    for (int i = 0; i < reportQueueItemCount;)
    {
      ReportQueue[i++]->reportToJSON();
      if (i != reportQueueItemCount)
        strcat_P(LongMessage, (PGM_P)F(",")); ///< < Unless it was the last element add a , separator
    }
    strcat_P(LongMessage, (PGM_P)F("}}")); ///< closing both curly bracket
    relayToGoogleSheets(&LongMessage);
  }
}
///< This is how a sent out message looks like:
///< {parameter={Log={"Report":{"InternalTemp":"20.84","ExternalTemp":"20.87","InternalHumidity":"38.54","ExternalHumidity":"41.87","InternalFan":"0","ExhaustFan":"0","Lt1_Status":"0","Lt1_Brightness":"15","LightReading":"454","Dark":"1","WaterLevel":"0","WaterTemp":"20.56","PH":"17.73","Pressure":"-0.18","Power":"-1.00","Energy":"-0.00","Voltage":"-1.00","Current":"-1.00","Lt1_Timer":"1","Lt1_OnTime":"04:20","Lt1_OffTime":"16:20","AeroInterval":"15","AeroDuration":"2"},"Settings":{"Metric":"1"}}}, contextPath=, contentLength=499, queryString=, parameters={Log=[Ljava.lang.Object;@60efa46b}, postData=FileUpload}

void MainModule::setPushingBoxLogRelayID(const char *ID)
{
  strncpy(ModuleSettings->PushingBoxLogRelayID, ID, MaxWordLength);
  getSoundObject()->playOnSound();
  addToLog(F("Sheets log relay ID updated"));
}

/*
void MainModule::relayToGoogleSheets(const __FlashStringHelper *Title, char (*JSONData)[MaxLongTextLength])
{
  char ValueToReport[MaxLongTextLength] = "";
  strcat_P(ValueToReport, (PGM_P)F("/pushingbox?devid="));
  strcat(ValueToReport, ModuleSettings -> PushingBoxLogRelayID);
  strcat_P(ValueToReport, (PGM_P)F("&BoxData={\""));
  strcat_P(ValueToReport, (PGM_P)Title);
  strcat_P(ValueToReport, (PGM_P)F("\":"));
  strcat(ValueToReport, *JSONData);
  strcat_P(ValueToReport, (PGM_P)F("}"));
  if (*Debug)
  { ///< print the report command to console
    logToSerials(F("api.pushingbox.com"), false, 4);
    logToSerials(&ValueToReport, true, 0);
  }
  PushingBoxRestAPI.get(ValueToReport); ///< PushingBoxRestAPI will append http:///< api.pushingbox.com/ in front of the command
}
*/