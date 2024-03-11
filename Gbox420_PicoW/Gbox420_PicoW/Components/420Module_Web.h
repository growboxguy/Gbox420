#pragma once

/*
#include "Arduino.h"
#include "ELClient.h"          ///< ESP-link
#include "ELClientWebServer.h" ///< ESP-link - WebServer API
#include "ELClientRest.h"      ///< ESP-link - REST API
#include "ELClientMqtt.h"      // ESP-link - MQTT protocol for sending and receiving IoT messages
#include "ELClientCmd.h"       ///< ESP-link - Get current time from the internet using NTP
#include "TimeLib.h"           // Keeping track of time
#include "SPI.h"
*/

#include "../Settings.h"
#include "Helpers.h"
#include "420Common_Web.h"
#include "420Module.h"

///< Extends the Module class with functions to interact with an HTTP / HTML based user interface hosted by the Main module

// extern ELClientWebServer WebServer;
// extern ELClientRest PushingBoxRestAPI;
// extern ELClientMqtt MqttAPI;
// extern bool MqttConnected;
class Sound;

class Module_Web : virtual public Common_Web, virtual public Module
{
public:
  Module_Web(const char *Name);                                       ///< constructor
  void addToWebsiteQueue_Load(Common_Web *Subscriber);                ///< Calls the websiteEvent_Load() method of the Subscriber when an ESP-link website is loaded
  void addToWebsiteQueue_Refresh(Common_Web *Subscriber);             ///< Calls the websiteEvent_Refresh() method of the Subscriber when an ESP-link website is refreshing
  void websiteLoadEventTrigger(__attribute__((unused)) char *Url);    ///< Notifies the subscribed components of a Load event. Passes the URL of the custom webpage that was opened (/Hempy.html or /Settings.html )
  void websiteRefreshEventTrigger(__attribute__((unused)) char *Url); ///< Notifies the subscribed components of a Refresh event. Passes the URL of the custom webpage that was opened (/Hempy.html or /Settings.html )
  void refresh();
  char *settingsToJSON();
  void settingsEvent_Load(__attribute__((unused)) char *Url);                                            ///< Gets called when the /Settings.html is loaded. This page is for configuring the Gbox420 module settings (Console logging, Debug mode, MQTT reporting topic, Google Sheets relay...etc)
  void settingsEvent_Refresh(__attribute__((unused)) char *Url);                                         ///< Gets called when the /Settings.html is refreshed.
  void settingsEvent_Command(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data); ///< Gets called a button is clicked or a field is submitted on the /Settings.html page
  void addToLog(const char *Text, uint8_t Indent = 3);                                                   ///< Add a Log entry that is displayed on the web interface
  char *eventLogToJSON(bool IncludeKey = false, bool ClearBuffer = true);                                ///< Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"] and loads it to LongMessage buffer
  void addPushingBoxLogRelayID();                                                                        ///< Google Sheets reporting - Set PushingBox relay ID
  void relayToGoogleSheets(char (*JSONData)[MaxLongTextLength]);                                         ///< Google Sheets reporting - Send a JSON formatted report via REST API to the PushingBox relay
  void reportToGoogleSheetsTrigger(bool ForceRun = false);                                               ///< Google Sheets reporting - Handles custom reporting frequencies
  void mqttPublish(char (*JSONData)[MaxLongTextLength]);                                                 ///< MQTT reporting - Send a JSON formatted report to an MQTT broker
  void reportToMQTTTrigger(bool ForceRun = false);                                                       ///< MQTT reporting - Handles custom reporting frequencies
  Sound *SoundFeedback = NULL;
  Sound *getSoundObject();

private:
protected:
  void setDebug(bool DebugEnabled);
  char *getDebugText(bool FriendlyFormat = false);
  void setMetric(bool MetricEnabled);
  char *getMetricText(bool FriendlyFormat = false);
  void setSheetsReportingOnOff(bool State);
  void setSheetsReportingFrequency(uint16_t Frequency);
  void setPushingBoxLogRelayID(const char *ID);
  Common_Web *WebsiteQueue_Load[QueueDepth] = {};
  Common_Web *WebsiteQueue_Refresh[QueueDepth] = {};
  uint8_t WebsiteQueue_Load_Count = 0;
  uint8_t WebsiteQueue_Refresh_Count = 0;
  bool RefreshRequested = false;
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