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
  logToSerials(&command, false, 1);
  logToSerials(&data, true, 2);
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
* @brief Loads the PushingBox device ID to the LongMessage buffer. Pushingbox is a relay service that is used to send reports to Google Sheets: https://sites.google.com/site/growboxguy/esp-link/logging
* Example
* /pushingbox?devid=v755877CF53383E1&BoxData=
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
* Example
* REST API reporting: api.pushingbox.com/pushingbox?devid=v755877CF53383E1&BoxData={"Log":{"IFan":{"S":"1"},"EFan":{"S":"1"},"APump1":{"S":"1"},"Lt1":{"S":"1","CB":"85","B":"85","T":"1","On":"14:20","Of":"02:20"},"Lt2":{"S":"1","CB":"95","B":"95","T":"1","On":"10:20","Of":"02:20"},"LtSen1":{"R":"959","D":"0"},"DHT1":{"T":"26.70","H":"44.50"},"Pow1":{"P":"572.20","E":"665.26","V":"228.00","C":"2.62","F":"50.00","PF":"0.96"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","PT1":"120","DT1":"300","WB1":"21.44","WR1":"6.85","DW1":"19.00","WW1":"0.00","ET1":"2.00","OT1":"0.20","WL1":"13.00","H2":"1","P2":"1","PS2":"100","PT2":"120","DT2":"300","WB2":"19.71","WR2":"5.28","DW2":"19.30","WW2":"0.00","ET2":"2.00","OT2":"0.20","WL2":"13.00"},"Aero1":{"S":"1","P":"5.68","W":"23.31","Mi":"5.00","Ma":"7.00","AS":"1","LS":"5.77","PSt":"1","PS":"100","PT":"420","PP":"10","SE":"1","D":"3.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"2.28","T":"1082.70","W":"14.63","WT":"19.13","AT":"27.20","H":"37.90"},"Main1":{"M":"1","D":"0","RD":"0","RM":"0","RT":"0","RJ":"0"}}}
*/
void Module_Web::relayToGoogleSheets(char (*JSONData)[MaxLongTextLength])
{
  if (*Debug)
  {
    logToSerials(F("REST API reporting: api.pushingbox.com"), false, 2);
    logToSerials(JSONData, true, 0);
  }
  PushingBoxRestAPI.get(*JSONData); ///< PushingBoxRestAPI will append http://api.pushingbox.com/ in front of the command
}

/**
* @brief Publish an MQTT message containing a JSON formatted report
* Examples:
* MQTT reporting: Gbox420/{"Log":{"IFan":{"S":"1"},"EFan":{"S":"1"},"APump1":{"S":"1"},"Lt1":{"S":"1","CB":"85","B":"85","T":"1","On":"14:20","Of":"02:20"},"Lt2":{"S":"1","CB":"95","B":"95","T":"1","On":"10:20","Of":"02:20"},"LtSen1":{"R":"959","D":"0"},"DHT1":{"T":"26.70","H":"45.20"},"Pow1":{"P":"573.60","E":"665.47","V":"227.20","C":"2.64","F":"50.00","PF":"0.96"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","PT1":"120","DT1":"300","WB1":"21.45","WR1":"6.77","DW1":"19.00","WW1":"0.00","ET1":"2.00","OT1":"0.20","WL1":"13.00","H2":"1","P2":"1","PS2":"100","PT2":"120","DT2":"300","WB2":"19.69","WR2":"5.31","DW2":"19.30","WW2":"0.00","ET2":"2.00","OT2":"0.20","WL2":"13.00"},"Aero1":{"S":"1","P":"5.41","W":"23.57","Mi":"5.00","Ma":"7.00","AS":"1","LS":"5.50","PSt":"1","PS":"100","PT":"420","PP":"10","SE":"1","D":"3.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"2.41","T":"1071.30","W":"14.64","WT":"19.13","AT":"27.30","H":"38.50"},"Main1":{"M":"1","D":"0","RD":"0","RM":"1","RT":"0","RJ":"1"}}}
* MQTT reporting: Gbox420/{"EventLog":["","","Module initialized","Lt2 brightness updated"]}
*/
void Module_Web::mqttPublish(char (*JSONData)[MaxLongTextLength])
{
  if (*Debug)
  {
    logToSerials(F("MQTT reporting:"), false, 2);
    logToSerials(&(ModuleSettings->MqttPubTopic), false, 1);
    logToSerials(JSONData, true, 0);
  }
  MqttAPI.publish(ModuleSettings->MqttPubTopic, *JSONData, 0, 1); //(topic,message,qos (Only level 0 supported),retain )
}