#include "Hempy_Standalone.h"
#include "Sound.h"
/*
#include "DHTSensor.h"
#include "WeightSensor.h"
#include "WaterPump.h"
#include "WasteReservoir.h"
#include "HempyBucket.h"
*/

/**
 * @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
 */
Hempy_Standalone::Hempy_Standalone(const char *Name, Settings::Hempy_StandaloneSettings *DefaultSettings, Settings::MqttClientSettings *MqttSettings) : Common(Name), Module(Name)
{
  SerialReportFrequency = &DefaultSettings->SerialReportFrequency;
  SerialReportDate = &DefaultSettings->SerialReportDate;
  SerialReportMemory = &DefaultSettings->SerialReportMemory;
  SerialReportJSONFriendly = &DefaultSettings->SerialReportJSONFriendly;
  SerialReportJSON = &DefaultSettings->SerialReportJSON;
  SerialReportWireless = &DefaultSettings->SerialReportWireless;
  ReportToGoogleSheets = &DefaultSettings->ReportToGoogleSheets;
  ReportToMqtt = &DefaultSettings->ReportToMqtt;
  SheetsReportingFrequency = &DefaultSettings->SheetsReportingFrequency;  


  Sound1 = new Sound(this, &ModuleSettings->Sound1); ///< Passing DefaultSettings members as references: Changes get written back to DefaultSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&DefaultSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  MosquittoMqtt = new MqttClient(&ModuleSettings->HempyMqttServer1, (void *)mqttDataReceived);
  this->DefaultMqttClient = MosquittoMqtt;

  addToReportQueue(this);  //< Attach to the report event: When triggered the module reports to the Serial Console or to MQTT
  addToRefreshQueue_1sec(this); //< Attach to a trigger that fires every second and calls refresh()
  addToRefreshQueue_5sec(this); //< Attach to a trigger that fires every second and calls refresh()
  addToRefreshQueue_1min(this); //< Attach to a trigger that fires every second and calls refresh()
  // addToWebsiteQueue_Load(this);    //< Attach to the ESP-link website load event: Calls websiteEvent_Load() when an ESP-link webpage is opened
  // addToWebsiteQueue_Refresh(this); //< Attach to the ESP-link website refresh event: Calls websiteEvent_Refresh() when an ESP-link webpage is refreshing
  addToCommandQueue(this);
  addToLog("Hempy_Standalone ready", 0);
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
  // WebServer.setArgString("Time", getCurrentTime(false));  ///< Current time
  // WebServer.setArgJson("Log", eventLogToJSON(false, true)); ///< Last events that happened in JSON format
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
    return true; // Match found
  }
}