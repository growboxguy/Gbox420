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
#include "../Components_Web/420Common_Web.h"
#include "../Components/420Module.h"

///< Extends the Module class with functions to interact with an HTTP / HTML based user interface hosted by the Main module 

extern ELClientWebServer WebServer;
extern ELClientRest PushingBoxRestAPI;
extern ELClientMqtt MqttAPI;
extern bool MqttConnected;
class Sound_Web;

class Module_Web : virtual public Common_Web, virtual public Module
{
public:
  Module_Web(const __FlashStringHelper *Name);                        ///< constructor
  void addToWebsiteQueue_Load(Common_Web *Subscriber);                ///< Calls the websiteEvent_Load() method of the Subscriber when an ESP-link website is loaded
  void addToWebsiteQueue_Refresh(Common_Web *Subscriber);             ///< Calls the websiteEvent_Refresh() method of the Subscriber when an ESP-link website is refreshing
  void addToCommandQueue(Common_Web *Subscriber);                     ///< Subscribing to commands from external systems (MQTT, HTTP): Calls the commandEvent() method
  void websiteLoadEventTrigger(__attribute__((unused)) char *Url);    ///< Notifies the subscribed components of a Load event. Passes the URL of the custom webpage that was opened (/Hempy.html or /Settings.html )
  void websiteRefreshEventTrigger(__attribute__((unused)) char *Url); ///< Notifies the subscribed components of a Refresh event. Passes the URL of the custom webpage that was opened (/Hempy.html or /Settings.html )
  void commandEventTrigger(char *Command, char *Data);                ///< Notifies the subscribed components of an incoming command. Command: combination of the Name of the component and a command (like Pump1_On, Light1_Brightness). Data: Optional value, passed as a character array (can be parsed to int/float/boolean)
  void refresh_FiveSec();
  void refresh_Minute();
  char * settingsToJSON();
  void settingsEvent_Load(__attribute__((unused)) char *Url);                                            ///< Gets called when the /Settings.html is loaded. This page is for configuring the Gbox420 module settings (Console logging, Debug mode, MQTT reporting topic, Google Sheets relay...etc)
  void settingsEvent_Refresh(__attribute__((unused)) char *Url);                                         ///< Gets called when the /Settings.html is refreshed.
  void settingsEvent_Command(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data); ///< Gets called a button is clicked or a field is submitted on the /Settings.html page
  void addToLog(const __FlashStringHelper *Text, uint8_t Indent = 3);                                    ///< Add a Log entry that is displayed on the web interface
  void addToLog(const char *Text, uint8_t Indent = 3);                                                   ///< Add a Log entry that is displayed on the web interface
  char *eventLogToJSON(bool IncludeKey = false, bool ClearBuffer = true);                                ///< Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"] and loads it to LongMessage buffer
  void addPushingBoxLogRelayID();                                                                        ///< Google Sheets reporting - Set PushingBox relay ID
  void relayToGoogleSheets(char (*JSONData)[MaxLongTextLength]);                                         ///< Google Sheets reporting - Send a JSON formatted report via REST API to the PushingBox relay
  void reportToGoogleSheetsTrigger(bool ForceRun = false);                                               ///< Google Sheets reporting - Handles custom reporting frequencies
  void mqttPublish(char (*JSONData)[MaxLongTextLength]);                                                 ///< MQTT reporting - Send a JSON formatted report to an MQTT broker
  void reportToMQTTTrigger(bool ForceRun = false);                                                       ///< MQTT reporting - Handles custom reporting frequencies
  Sound_Web *SoundFeedback = NULL;
  Sound_Web *getSoundObject();

private:
protected:
  void setDebug(bool DebugEnabled);
  char *getDebugText(bool FriendlyFormat = false);
  void setMetric(bool MetricEnabled);
  char *getMetricText(bool FriendlyFormat = false);
  void setSheetsReportingOnOff(bool State);
  void setSheetsReportingFrequency(uint16_t Frequency);
  void setPushingBoxLogRelayID(const char *ID);
  void setMQTTReportingOnOff(bool State);
  void setMQTTReportingFrequency(uint16_t Frequency);
  void setMqttPublishTopic(const char *ID);
  void setMqttSubscribeTopic(const char *ID);
  void setMQTTLWTTopic(const char *ID);
  void setMQTTLWTMessage(const char *ID);
  Common_Web *WebsiteQueue_Load[QueueDepth] = {};
  Common_Web *WebsiteQueue_Refresh[QueueDepth] = {};
  Common_Web *CommandQueue[QueueDepth] = {};
  uint8_t WebsiteQueue_Load_Count = 0;
  uint8_t WebsiteQueue_Refresh_Count = 0;
  uint8_t CommandQueue_Count = 0;
  bool RefreshAllRequested = false;
  bool ConsoleReportRequested = false;
  bool ReportToGoogleSheetsRequested = false;
  bool MQTTReportRequested = false;
  bool *ReportToGoogleSheets;
  uint16_t *SheetsReportingFrequency;
  uint8_t SheetsTriggerCounter = 0;
  bool *ReportToMQTT;
  uint16_t *MQTTReportFrequency;
  uint16_t MQTTTriggerCounter = 0;
};