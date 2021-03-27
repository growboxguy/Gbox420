#include "420Module_Web.h"
#include "../Components/Sound.h"

static char Logs[LogDepth][MaxWordLength]; ///< two dimensional array for storing log histroy displayed on the website (array of char arrays)

/**
* @brief Constructor: creates an instance of the class, and stores wireless transmitter object used to communicate with other modules
*/
Module_Web::Module_Web(RF24 *Wireless) : Common(Name), Module()
{
  this->Wireless = Wireless;
  //logToSerials(F("Module_Web ready"), true, 3);
}

/**
* @brief Subscribe to the ESP-link website load event
*/
void Module_Web::addToWebsiteQueue_Load(Common_Web *Module)
{
  if (QueueDepth > WebsiteQueue_Load_Count)
    WebsiteQueue_Load[WebsiteQueue_Load_Count++] = Module;
  else
    logToSerials(F("WebsiteQueue_Load overflow!"), true, 0);
}

/**
* @brief Subscribe to the ESP-link website refresh event
*/
void Module_Web::addToWebsiteQueue_Refresh(Common_Web *Module)
{
  if (QueueDepth > WebsiteQueue_Refresh_Count)
    WebsiteQueue_Refresh[WebsiteQueue_Refresh_Count++] = Module;
  else
    logToSerials(F("WebsiteQueue_Refresh overflow!"), true, 0);
}

/**
* @brief Subscribe to MQTT or ESP-link website commands  (Pressing a button, setting a value)
*/
void Module_Web::addToCommandQueue(Common_Web *Module)
{
  if (QueueDepth > CommandQueue_Count)
    CommandQueue[CommandQueue_Count++] = Module;
  else
    logToSerials(F("CommandQueue overflow!"), true, 0);
}

/**
* @brief Notify subscribed modules of a website load event
*/
void Module_Web::websiteLoadEventTrigger(char *url)
{ ///< called when website is loaded. Runs through all components that subscribed for this event
  for (int i = 0; i < WebsiteQueue_Load_Count; i++)
  {
    WebsiteQueue_Load[i]->websiteEvent_Load(url);
  }
}

/**
* @brief Notify subscribed modules of a website refresh event
*/
void Module_Web::websiteRefreshEventTrigger(char *url)
{ ///< called when website is refreshed.
  for (int i = 0; i < WebsiteQueue_Refresh_Count; i++)
  {
    WebsiteQueue_Refresh[i]->websiteEvent_Refresh(url);
  }
}

/**
* @brief Notify subscribed modules of a received MQTT/Website command
*/
void Module_Web::commandEventTrigger(char *command, char *data)
{
  logToSerials(command, false, 0);
  logToSerials(data, true, 2);
  for (int i = 0; i < CommandQueue_Count; i++)
  {
    CommandQueue[i]->commandEvent(command, data);
  }
}

/**
* @brief Adds a log entry to the top of the log and removes the oldest log entry
*/
void Module_Web::addToLog(const char *LongMessage, __attribute__((unused)) uint8_t Indent)
{ ///< adds a log entry that is displayed on the web interface
  logToSerials(&LongMessage, true, Indent);
  for (uint8_t i = LogDepth - 1; i > 0; i--)
  {                                       ///< Shift every log entry one up, dropping the oldest
    memset(&Logs[i], 0, sizeof(Logs[i])); ///< clear variable
    strncpy(Logs[i], Logs[i - 1], MaxWordLength);
  }
  memset(&Logs[0], 0, sizeof(Logs[0]));         ///< clear variable
  strncpy(Logs[0], LongMessage, MaxWordLength); ///< instert new log to [0]
}

/**
* @brief Adds a log entry to the top of the log and removes the oldest log entry
*/
void Module_Web::addToLog(const __FlashStringHelper *LongMessage, __attribute__((unused)) uint8_t Indent)
{ ///< function overloading: same function name, different parameter type
  logToSerials(&LongMessage, true, Indent);
  for (uint8_t i = LogDepth - 1; i > 0; i--)
  {                                       ///< Shift every log entry one up, dropping the oldest
    memset(&Logs[i], 0, sizeof(Logs[i])); ///< clear variable
    strncpy(Logs[i], Logs[i - 1], MaxWordLength);
  }
  memset(&Logs[0], 0, sizeof(Logs[0]));                  ///< clear variable
  strncpy_P(Logs[0], (PGM_P)LongMessage, MaxWordLength); ///< instert new log to [0]
}

/**
* @brief Converts the log entries to a JSON object
* IncludeKey false: ["Log1","Log2","Log3",...,"LogN"], 
* IncludeKey true:  {"EventLog"\["Log1","Log2","Log3",...,"LogN"]}
* Append false: Clear the LongMessage buffer before
*/
char *Module_Web::eventLogToJSON(bool IncludeKey, bool ClearBuffer)
{
  if (ClearBuffer)
  {
    memset(&LongMessage[0], 0, MaxLongTextLength);
  }
  if (IncludeKey)
  {
    strcat_P(LongMessage, (PGM_P)F("{\"EventLog\":")); ///< Adds a curly bracket that needs to be closed at the end
  }
  strcat_P(LongMessage, (PGM_P)F("["));
  for (int i = LogDepth - 1; i >= 0; i--)
  {
    strcat_P(LongMessage, (PGM_P)F("\""));
    strcat(LongMessage, Logs[i]);
    strcat_P(LongMessage, (PGM_P)F("\""));
    if (i > 0)
      strcat_P(LongMessage, (PGM_P)F(","));
  }
  LongMessage[strlen(LongMessage)] = ']';
  if (IncludeKey)
  {
    strcat_P(LongMessage, (PGM_P)F("}")); ///< closing curly bracket
  }
  return LongMessage;
}


///< Google Sheets functions - https://sites.google.com/site/growboxguy/esp-link/logging

/**
* @brief Sets the PushingBox device ID that is used to send reports to Google Sheets
*/
void Module_Web::addPushingBoxLogRelayID()
{
  memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("/pushingbox?devid="));
  strcat(LongMessage, ModuleSettings->PushingBoxLogRelayID);
  strcat_P(LongMessage, (PGM_P)F("&BoxData="));
}

/**
* @brief Send a JSON formatted report to Google Sheets
*/
void Module_Web::relayToGoogleSheets(char (*JSONData)[MaxLongTextLength])
{
  if (*Debug)
  { ///< print the report command to console
    logToSerials(F("REST API reporting: api.pushingbox.com"), false, 2);
    logToSerials(JSONData, true, 0);
  }
  PushingBoxRestAPI.get(*JSONData); ///< PushingBoxRestAPI will append http://api.pushingbox.com/ in front of the command
}

/**
* @brief Publish an MQTT message containing a JSON formatted report
*/
void Module_Web::mqttPublish(char (*JSONData)[MaxLongTextLength])
{
  memset(&ShortMessage[0], 0, MaxShotTextLength); ///< clear variable
  strcat(ShortMessage, ModuleSettings->MqttPubTopic);
  if (*Debug)
  { ///< print the report command to console
    logToSerials(F("MQTT reporting:"), false, 2);
    logToSerials(&ShortMessage, false, 1);
    logToSerials(JSONData, true, 0);
  }
  MqttAPI.publish(ShortMessage, *JSONData, 0, 1); //(topic,message,qos (Only level 0 supported),retain )
}