#include "DevModule_Web.h"
#include "../Components_Web/Sound_Web.h"
#include "../Components_Web/DHTSensor_Web.h"

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
*/
DevModule_Web::DevModule_Web(const __FlashStringHelper *Name, Settings::DevModule_WebSettings *DefaultSettings, RF24 *Wireless) : Module_Web(Name), Module(Name), Common_Web(Name), Common(Name)
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

  logToSerials(F(""), true, 0);                                       //<Line break
  Sound1 = new Sound_Web(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;                                       ///< Pointer for child objects to use sound feedback
  DHT1 = new DHTSensor_Web(F("DHT1"), this, &ModuleSettings->DHT1);
  addToReportQueue(this);          //< Attach to the report event: When triggered the module reports to the Serial Console or to MQTT
  addToRefreshQueue_FiveSec(this); //< Attach to a trigger that fires every five seconds and calls refresh_FiveSec()
  addToRefreshQueue_Minute(this);  //< Attach to a trigger that fires every second and calls refresh_Minute()
  addToWebsiteQueue_Load(this);    //< Attach to the ESP-link website load event: Calls websiteEvent_Load() when an ESP-link webpage is opened
  addToWebsiteQueue_Refresh(this); //< Attach to the ESP-link website refresh event: Calls websiteEvent_Refresh() when an ESP-link webpage is refreshing
  addToCommandQueue(this);
  logToSerials(Name, false, 0);
  logToSerials(F("refreshing"), true, 1);
  runAll();
  addToLog(F("DevModule_Web initialized"), 0);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void DevModule_Web::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"M\":\""));
  strcat(LongMessage, getMetricText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, getDebugText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void DevModule_Web::websiteEvent_Load(char *url)
{
 ;
}

void DevModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///< called when website is refreshed.
{
  WebServer.setArgString(getName(F("Time")), getFormattedTime(false));
  WebServer.setArgJson(getName(F("Log")), eventLogToJSON(false, true)); ///< Last events that happened in JSON format
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
bool DevModule_Web::commandEvent(char *Command, char *Data)
{
  /*
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {    
    return true; //Match found
  }
  */
}

void DevModule_Web::refresh_FiveSec()
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

void DevModule_Web::refresh_Minute()
{
  Common::refresh_Minute();
  reportToGoogleSheetsTrigger();
}