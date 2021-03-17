#include "420Module_Web.h"
#include "../Components/Sound.h"

static char Logs[LogDepth][MaxWordLength]; ///< two dimensional array for storing log histroy displayed on the website (array of char arrays)

Module_Web::Module_Web(RF24 *Wireless) : Module()
{
  this->Wireless = Wireless;
  //logToSerials(F("Module_Web object created"), true, 3);
}

///< Website subscriptions: When a Module needs to get notified of a Website events from the ESP-link it subscribes to one or more website queues using these methods

void Module_Web::addToWebsiteQueue_Load(Common_Web *Module)
{
  if (QueueDepth > WebsiteQueue_Load_Count)
    WebsiteQueue_Load[WebsiteQueue_Load_Count++] = Module;
  else
    logToSerials(F("WebsiteQueue_Load overflow!"), true, 0);
}

void Module_Web::addToWebsiteQueue_Refresh(Common_Web *Module)
{
  if (QueueDepth > WebsiteQueue_Refresh_Count)
    WebsiteQueue_Refresh[WebsiteQueue_Refresh_Count++] = Module;
  else
    logToSerials(F("WebsiteQueue_Refresh overflow!"), true, 0);
}

void Module_Web::addToCommandQueue(Common_Web *Module)
{
  if (QueueDepth > CommandQueue_Count)
    CommandQueue[CommandQueue_Count++] = Module;
  else
    logToSerials(F("CommandQueue overflow!"), true, 0);
}

///< Website queues: Notify components in the Module_Web of a website event

void Module_Web::loadEvent(char *url)
{ ///< called when website is loaded. Runs through all components that subscribed for this event
  for (int i = 0; i < WebsiteQueue_Load_Count; i++)
  {
    WebsiteQueue_Load[i]->websiteEvent_Load(url);
  }
}

void Module_Web::refreshEvent(char *url)
{ ///< called when website is refreshed.
  for (int i = 0; i < WebsiteQueue_Refresh_Count; i++)
  {
    WebsiteQueue_Refresh[i]->websiteEvent_Refresh(url);
  }
}

void Module_Web::commandEvent(char *command, char *data)
{
  if (*Debug)
    logToSerials(&command, false, 0);
    logToSerials(&data, true, 2);
  for (int i = 0; i < CommandQueue_Count; i++)
  {
    CommandQueue[i]->commandEvent(command,data);
  }
}

///< Even logs on the website
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

char *Module_Web::eventLogToJSON(bool Append)
{ ///< Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  if (!Append)
    memset(&LongMessage[0], 0, sizeof(LongMessage));
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
  return LongMessage;
}

///< Google Sheets functions

void Module_Web::addPushingBoxLogRelayID()
{
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("/pushingbox?devid="));
  strcat(LongMessage, ModuleSettings->PushingBoxLogRelayID);
  strcat_P(LongMessage, (PGM_P)F("&BoxData="));
}

void Module_Web::relayToGoogleSheets(char (*JSONData)[MaxLongTextLength])
{
  if (*Debug)
  { ///< print the report command to console
    logToSerials(F("REST API reporting: api.pushingbox.com"), false, 2);
    logToSerials(JSONData, true, 0);
  }
  PushingBoxRestAPI.get(*JSONData); ///< PushingBoxRestAPI will append http://api.pushingbox.com/ in front of the command
}

void Module_Web::mqttPublish(char (*JSONData)[MaxLongTextLength])
{
  memset(&ShortMessage[0], 0, sizeof(ShortMessage)); ///< clear variable
  strcat(ShortMessage,ModuleSettings->MqttPubTopic);
  if (*Debug)
  { ///< print the report command to console
    logToSerials(F("MQTT reporting:"), false, 2);
    logToSerials(&ShortMessage,false,1);
    logToSerials(JSONData, true, 0);
  }
  MqttAPI.publish(ShortMessage, *JSONData,0,1); //(topic,message,qos (Only level 0 supported),retain )
}