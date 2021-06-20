#pragma once

#include "Arduino.h"
#include "ELClient.h"          ///< ESP-link
#include "ELClientWebServer.h" ///< ESP-link - WebServer API
#include "ELClientRest.h"      ///< ESP-link - REST API
#include "ELClientMqtt.h"      // ESP-link - MQTT protocol for sending and receiving IoT messages
#include "ELClientCmd.h"       ///< ESP-link - Get current time from the internet using NTP
#include "TimeLib.h"           // Keeping track of time
#include "SPI.h"
#include "../../Settings.h"
#include "../Helpers.h"
#include "../Components/420Module.h"
#include "420Common_Web.h"
///< This class represents a complete Module_Web with all of its components
///< Responsible for setting up each module, updating their statuses and reporting it

extern ELClientWebServer WebServer;
extern ELClientRest PushingBoxRestAPI;
extern ELClientMqtt MqttAPI;
extern bool MqttConnected;
class Sound;

class Module_Web : virtual public Common, virtual public Module
{
public:
  Module_Web(); ///< constructor
  void addToWebsiteQueue_Load(Common_Web *Module);    ///< Subscribing to the Website load event: Calls the websiteEvent_Load() method
  void addToWebsiteQueue_Refresh(Common_Web *Module); ///< Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  void addToCommandQueue(Common_Web *Module);         ///< Subscribing to commands from external systems (MQTT, HTTP): Calls the commandEvent method
  void websiteLoadEventTrigger(char *Url);
  void websiteRefreshEventTrigger(char *Url);
  void commandEventTrigger(char *command, char *data);
  void addToLog(const __FlashStringHelper *Text, uint8_t indent = 3);
  void addToLog(const char *Text, uint8_t indent = 3);
  char *eventLogToJSON(bool IncludeKey = false, bool ClearBuffer = true); ///< Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"] and loads it to LongMessage buffer
  void addPushingBoxLogRelayID();
  void relayToGoogleSheets(char (*JSONData)[MaxLongTextLength]);
  void mqttPublish(char (*JSONData)[MaxLongTextLength]);
  
private:
protected:
  Common_Web *WebsiteQueue_Load[QueueDepth] = {};
  Common_Web *WebsiteQueue_Refresh[QueueDepth] = {};
  Common_Web *CommandQueue[QueueDepth] = {};
  uint8_t WebsiteQueue_Load_Count = 0;
  uint8_t WebsiteQueue_Refresh_Count = 0;
  uint8_t CommandQueue_Count = 0;
};