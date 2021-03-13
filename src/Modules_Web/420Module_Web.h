#pragma once

#include "Arduino.h"
#include "ELClient.h"          ///< ESP-link
#include "ELClientWebServer.h" ///< ESP-link - WebServer API
#include "ELClientRest.h"      ///< ESP-link - REST API
#include "ELClientMqtt.h"      // ESP-link - MQTT protocol for sending and receiving IoT messages
#include "ELClientCmd.h"       ///< ESP-link - Get current time from the internet using NTP
#include "TimeLib.h"           // Keeping track of time
#include "RF24.h"
#include "SPI.h"
#include "../../Settings.h"
#include "../Helpers.h"
#include "../WirelessCommands_Hempy.h"
#include "../Components/420Module.h"
#include "420Common_Web.h"
///< This class represents a complete Module_Web with all of its components
///< Responsible for setting up each module, updating their statuses and reporting it

extern ELClientWebServer WebServer;
extern ELClientRest PushingBoxRestAPI;
extern ELClientMqtt MqttAPI;
class Sound;

class Module_Web : public Module
{
public:
  Module_Web(RF24 *Wireless); ///< constructor
  RF24 *Wireless;
  /*
  void runReport();
  void runAll();
  void runSec();
  void runFiveSec();
  void runMinute();
  */
  void addToWebsiteQueue_Load(Common_Web *Module);    ///< Subscribing to the Website load event: Calls the websiteEvent_Load() method
  void addToWebsiteQueue_Refresh(Common_Web *Module); ///< Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  void addToWebsiteQueue_Button(Common_Web *Module);  ///< Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  void addToWebsiteQueue_Field(Common_Web *Module);   ///< Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  void loadEvent(char *Url);
  void refreshEvent(char *Url);
  void buttonEvent(char *Button);
  void setFieldEvent(char *Field);
  //void addToReportQueue(Common *Component);
  //void addToRefreshQueue_Sec(Common *Component);
  //void addToRefreshQueue_FiveSec(Common *Component);
  //void addToRefreshQueue_Minute(Common *Component);
  void addToLog(const __FlashStringHelper *Text, uint8_t indent = 3);
  void addToLog(const char *Text, uint8_t indent = 3);
  char *eventLogToJSON(bool Append = false); ///< Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  char *getJSONReport(bool BlankLongMessage = false);  
  void addPushingBoxLogRelayID();
  void relayToGoogleSheets(char (*JSONData)[MaxLongTextLength]);
  void mqttPublish(char (*JSONData)[MaxLongTextLength]);

private:
protected:
  //Common *ReportQueue[QueueDepth] = {}; ///< aggregate initializer: Same as initializing to null pointers
  //Common *RefreshQueue_Sec[QueueDepth] = {};
  //Common *RefreshQueue_FiveSec[QueueDepth] = {};
  //Common *RefreshQueue_Minute[QueueDepth] = {};
  Common_Web *WebsiteQueue_Load[QueueDepth] = {};
  Common_Web *WebsiteQueue_Refresh[QueueDepth] = {};
  Common_Web *WebsiteQueue_Button[QueueDepth] = {};
  Common_Web *WebsiteQueue_Field[QueueDepth] = {};
  //uint8_t reportQueueItemCount = 0; ///< Tracking queue item count
  //uint8_t refreshQueueItemCount_Sec = 0;
  //uint8_t refreshQueueItemCount_FiveSec = 0;
  //uint8_t refreshQueueItemCount_Minute = 0;
  uint8_t WebsiteQueue_Load_Count = 0;
  uint8_t WebsiteQueue_Refresh_Count = 0;
  uint8_t WebsiteQueue_Button_Count = 0;
  uint8_t WebsiteQueue_Field_Count = 0;
};