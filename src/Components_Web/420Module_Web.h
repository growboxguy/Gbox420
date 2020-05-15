#pragma once

#include "Arduino.h"     
#include "ELClient.h"          ///ESP-link
#include "ELClientWebServer.h" ///ESP-link - WebServer API
#include "ELClientRest.h" ///ESP-link - REST API
#include "ELClientCmd.h"       ///ESP-link - Get current time from the internet using NTP
#include "TimeLib.h"           ///Keeping track of time
#include "RF24.h"
#include "SPI.h"
#include "nRF24L01.h"
#include "../../Settings.h"
#include "../Helpers.h"
#include "../WirelessCommands_Hempy.h"
#include "../Components/420Module.h"
#include "420Common_Web.h"

///This class represents a complete Module_Web with all of its components
///Responsible for setting up each module, updating their statuses and reporting it

extern ELClientRest PushingBoxRestAPI;
class Sound_Web;

class Module_Web : public Module
{
public:
  Module_Web(RF24 *Wireless); ///constructor
  void runReport();
  void runAll();
  void runSec();
  void runFiveSec();
  void runMinute();
  void runQuarterHour();   

  void addToWebsiteQueue_Load(Common_Web *Component);   ///< Subscribing to the Website load event: Calls the websiteEvent_Load() method
  void addToWebsiteQueue_Refresh(Common_Web *Component);  ///< Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  void addToWebsiteQueue_Button(Common_Web *Component);   ///< Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  void addToWebsiteQueue_Field(Common_Web *Component);    ///< Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  void loadEvent(char *Url);
  void refreshEvent(char *Url);
  void buttonEvent(char *Button);
  void setFieldEvent(char *Field); 
  void addToReportQueue(Common_Web *Component);
  void addToRefreshQueue_Sec(Common_Web *Component);
  void addToRefreshQueue_FiveSec(Common_Web *Component);
  void addToRefreshQueue_Minute(Common_Web *Component);
  void addToRefreshQueue_QuarterHour(Common_Web *Component);
  void addToLog(const __FlashStringHelper *Text, uint8_t indent = 3);
  void addToLog(const char *Text, uint8_t indent = 3); 
  char *eventLogToJSON(bool Append = false); ///Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  RF24 *Wireless;
  
  void addPushingBoxLogRelayID();
  void relayToGoogleSheets(char (*JSONData)[MaxLongTextLength]);

private:

protected:   
  Common_Web *ReportQueue[QueueDepth] = {};  ///aggregate initializer: Same as initializing to null pointers
  Common_Web *RefreshQueue_Sec[QueueDepth]= {};
  Common_Web *RefreshQueue_FiveSec[QueueDepth]= {};
  Common_Web *RefreshQueue_Minute[QueueDepth]= {};
  Common_Web *RefreshQueue_QuarterHour[QueueDepth]= {};
  Common_Web *WebsiteQueue_Load[QueueDepth] = {};
  Common_Web *WebsiteQueue_Refresh[QueueDepth]= {};
  Common_Web *WebsiteQueue_Button[QueueDepth]= {};
  Common_Web *WebsiteQueue_Field[QueueDepth]= {}; 
  uint8_t reportQueueItemCount = 0; ///Tracking queue item count
  uint8_t refreshQueueItemCount_Sec = 0;
  uint8_t refreshQueueItemCount_FiveSec = 0;
  uint8_t refreshQueueItemCount_Minute = 0;
  uint8_t refreshQueueItemCount_QuarterHour = 0; 
  uint8_t WebsiteQueueItemCount_Load = 0;
  uint8_t WebsiteQueueItemCount_Refresh = 0;
  uint8_t WebsiteQueueItemCount_Button = 0;
  uint8_t WebsiteQueueItemCount_Field = 0;
};