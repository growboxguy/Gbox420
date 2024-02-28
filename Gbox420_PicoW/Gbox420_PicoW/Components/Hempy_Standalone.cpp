#include "Hempy_Standalone.h"
#include "Sound_Web.h"
/*
#include "DHTSensor_Web.h"
#include "WeightSensor_Web.h"
#include "WaterPump_Web.h"
#include "WasteReservoir_Web.h"
#include "HempyBucket_Web.h"
*/

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
*/
Hempy_Standalone::Hempy_Standalone(const char *Name, Settings::Hempy_StandaloneSettings *DefaultSettings) : Common(Name), Common_Web(Name), Module(Name), Module_Web(Name)
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

  printf("\n");                                                          //<Line break
  Sound1 = new Sound_Web("Sound1", this, &ModuleSettings->Sound1);                    ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))

  addToReportQueue(this);                                                                //< Attach to the report event: When triggered the module reports to the Serial Console or to MQTT
  //addToRefreshQueue_Sec(this);     //< Attach to a trigger that fires every second and calls refresh_Sec()
  addToRefreshQueue_FiveSec(this); //< Attach to a trigger that fires every five seconds and calls refresh_FiveSec()
  addToRefreshQueue_Minute(this);  //< Attach to a trigger that fires every second and calls refresh_Minute()
  addToWebsiteQueue_Load(this);    //< Attach to the ESP-link website load event: Calls websiteEvent_Load() when an ESP-link webpage is opened
  addToWebsiteQueue_Refresh(this); //< Attach to the ESP-link website refresh event: Calls websiteEvent_Refresh() when an ESP-link webpage is refreshing
  addToCommandQueue(this);
  addToLog("Hempy_Standalone ready", 0);
  printf(Name);
  printf(" refreshing\n");
  runAll();
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void Hempy_Standalone::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat(LongMessage, "\"M\":\"");
  strcat(LongMessage, getMetricText(FriendlyFormat));
  strcat(LongMessage, "\",\"D\":\"");
  strcat(LongMessage, getDebugText(FriendlyFormat));
  strcat(LongMessage, "\"}"); ///< closing the curly bracket at the end of the JSON
}

/**
* @brief Called when an ESP-link website is loading
*/
void Hempy_Standalone::websiteEvent_Load(__attribute__((unused)) char *Url) ///< called when website is first opened
{
  ;
}

/**
* @brief Called when an ESP-link website is refreshing
*/
void Hempy_Standalone::websiteEvent_Refresh(__attribute__((unused)) char *Url) ///< called when website is refreshed (5sec automatic)
{
  //WebServer.setArgString("Time", getFormattedTime(false));  ///< Current time
  //WebServer.setArgJson("Log", eventLogToJSON(false, true)); ///< Last events that happened in JSON format
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
bool Hempy_Standalone::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    return true; //Match found
  }
}

void Hempy_Standalone::refresh_FiveSec()
{
  Module_Web::refresh_FiveSec();
}

void Hempy_Standalone::refresh_Minute()
{
  Module_Web::refresh_Minute();
}