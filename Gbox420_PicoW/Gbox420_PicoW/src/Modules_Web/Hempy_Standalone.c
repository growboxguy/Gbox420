#include "Hempy_Standalone.h"
#include "../Components_Web/Sound_Web.h"
#include "../Components_Web/DHTSensor_Web.h"
#include "../Components_Web/WeightSensor_Web.h"
#include "../Components_Web/WaterPump_Web.h"
#include "../Components_Web/WasteReservoir_Web.h"
#include "../Components_Web/HempyBucket_Web.h"

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
*/
Hempy_Standalone::Hempy_Standalone(const __FlashStringHelper *Name, Settings::Hempy_StandaloneSettings *DefaultSettings) : Common(Name), Common_Web(Name), Module(Name), Module_Web(Name)
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

  logToSerials(F(""), true, 0);                                                          //<Line break
  Sound1 = new Sound_Web(F("Sound1"), this, &ModuleSettings->Sound1);                    ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))

  addToReportQueue(this);                                                                //< Attach to the report event: When triggered the module reports to the Serial Console or to MQTT
  //addToRefreshQueue_Sec(this);     //< Attach to a trigger that fires every second and calls refresh_Sec()
  addToRefreshQueue_FiveSec(this); //< Attach to a trigger that fires every five seconds and calls refresh_FiveSec()
  addToRefreshQueue_Minute(this);  //< Attach to a trigger that fires every second and calls refresh_Minute()
  addToWebsiteQueue_Load(this);    //< Attach to the ESP-link website load event: Calls websiteEvent_Load() when an ESP-link webpage is opened
  addToWebsiteQueue_Refresh(this); //< Attach to the ESP-link website refresh event: Calls websiteEvent_Refresh() when an ESP-link webpage is refreshing
  addToCommandQueue(this);
  addToLog(F("Hempy_Standalone ready"), 0);
  logToSerials(Name, false, 0);
  logToSerials(F("refreshing"), true, 1);
  runAll();
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void Hempy_Standalone::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"M\":\""));
  strcat(LongMessage, getMetricText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, getDebugText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
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
  WebServer.setArgString(F("Time"), getFormattedTime(false));  ///< Current time
  WebServer.setArgJson(F("Log"), eventLogToJSON(false, true)); ///< Last events that happened in JSON format
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