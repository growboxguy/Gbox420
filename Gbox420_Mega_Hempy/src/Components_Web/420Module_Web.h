#pragma once

#include "Arduino.h"
#include "ELClient.h"          ///< ESP-link
#include "ELClientWebServer.h" ///< ESP-link - WebServer API
#include "ELClientRest.h"      ///< ESP-link - REST API
#include "ELClientMqtt.h"      ///< ESP-link - MQTT protocol for sending and receiving IoT messages
#include "ELClientCmd.h"       ///< ESP-link - Get current time from the internet using NTP
#include "TimeLib.h"           ///< Keeping track of time
#include "../../Settings.h"
#include "../Helpers.h"
#include "../Components/420Module.h"
#include "420Common_Web.h"

///< This class represents a complete Module_Web with all of its components
///< Responsible for setting up each module, updating their statuses and reporting it

extern ELClientRest PushingBoxRestAPI;
extern ELClientMqtt MqttAPI;

class Module_Web : public Module
{
public:
  Module_Web(const __FlashStringHelper *Name); ///< constructor
  void addToWebsiteQueue_Load(Common_Web *Component);    ///< Subscribing to the Website load event: Calls the websiteEvent_Load() method
  void addToWebsiteQueue_Refresh(Common_Web *Component); ///< Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  void addToWebsiteQueue_Button(Common_Web *Component);  ///< Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  void addToWebsiteQueue_Field(Common_Web *Component);   ///< Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  void loadEvent(char *Url);
  void refreshEvent(char *Url);
  void buttonEvent(char *Button);
  void setFieldEvent(char *Field);  

private:
protected:
  Common_Web *WebsiteQueue_Load[QueueDepth] = {};  ///< aggregate initializer: Same as initializing to null pointers
  Common_Web *WebsiteQueue_Refresh[QueueDepth] = {};
  Common_Web *WebsiteQueue_Button[QueueDepth] = {};
  Common_Web *WebsiteQueue_Field[QueueDepth] = {};  
  uint8_t WebsiteQueue_Load_Count = 0;
  uint8_t WebsiteQueue_Refresh_Count = 0;
  uint8_t WebsiteQueue_Button_Count = 0;
  uint8_t WebsiteQueue_Field_Count = 0;
};