/*! \file
 *  \brief     Parent class of all Modules, provides a set of shared functions and variables to Modules
 *  \details   Functions to interact with MQTT/HTTP, report to stdout and schedule refresh triggers
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/
 *  \version   4.20
 */

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
#include "420Common.h"
#include "../Settings.h"
#include "Helpers.h"

///< Common parent class of all modules
///<

// extern ELClientWebServer WebServer;
// extern ELClientRest PushingBoxRestAPI;
// extern ELClientMqtt MqttAPI;
// extern bool MqttConnected;

class Sound;

class Module : virtual public Common
{
public:
  Module(const char *Name, Sound *SoundFeedback);                                                         ///< constructor
  Module(const char *Name);                                                                               ///< constructor
  static void mqttDataReceived(const char *Topic, uint16_t TopicLength, char *Data, uint16_t DataLength); ///< MQTT data received from the Subscribed topic
  void reportToSerialTrigger(bool ForceRun = false, bool ClearBuffer = true, bool KeepBuffer = false, bool JSONToBufferOnly = false);
  void commandEventTrigger(char *Command, char *Data);                                                            ///< Notifies the subscribed components of an incoming command. Command: combination of the Name of the component and a command (like Pump1_On, Light1_Brightness). Data: Optional value, passed as a character array (can be parsed to int/float/boolean)
  void runReport(bool ForceRun = false, bool ClearBuffer = true, bool KeepBuffer = false, bool JSONOnly = false); ///< Generate a text log of all sensor readings to the Serial output and/or to the LongMessage buffer.
  void addToCommandQueue(Common *Subscriber);                                                                     ///< Subscribing to commands from external systems (MQTT, HTTP): Calls the commandEvent() method
  void run1sec();                                                                                                 ///< Called every second
  void run5sec();                                                                                                 ///< Called every 5 seconds
  void run1min();                                                                                                 ///< Called every minute
  void addToLog(const char *Text, uint8_t Indent = 3); ///< Add a Log entry that is displayed on the web interface
  void addToReportQueue(Common *Component);  ///< Subscribing to the report queue: Calls the report() method
  void addToRefreshQueue(Common *Component); ///< Subscribing to the 1 sec refresh queue: Calls the refresh() method
  char *getFormattedTime(bool PrintToSerials);
  Sound *SoundFeedback = NULL;
  Sound *getSoundObject();
  uint16_t *SerialReportFrequency;   ///< Frequency of Serial reports in seconds
  uint16_t SerialTriggerCounter = 0; ///< Helps with timing when to send the Serial report out
  bool *SerialReportDate;            ///< Enable/disable reporting the current time to the Serial output
  bool *SerialReportMemory;          ///< Enable/disable reporting the remaining free memory to the Serial output
  bool *SerialReportJSONFriendly;    ///< Enable/disable sending Text formatted reports to the Serial output
  bool *SerialReportJSON;            ///< Enable/disable sending JSON formatted reports to the Serial output
  bool *SerialReportWireless;        ///< Enable/disable sending wireless package exchange reports to the Serial output

  void addToWebsiteQueue_Load(Common *Subscriber);                    ///< Calls the websiteEvent_Load() method of the Subscriber when an ESP-link website is loaded
  void addToWebsiteQueue_Refresh(Common *Subscriber);                 ///< Calls the websiteEvent_Refresh() method of the Subscriber when an ESP-link website is refreshing
  void websiteLoadEventTrigger(__attribute__((unused)) char *Url);    ///< Notifies the subscribed components of a Load event. Passes the URL of the custom webpage that was opened (/Hempy.html or /Settings.html )
  void websiteRefreshEventTrigger(__attribute__((unused)) char *Url); ///< Notifies the subscribed components of a Refresh event. Passes the URL of the custom webpage that was opened (/Hempy.html or /Settings.html )
  void refresh();
  char *settingsToJSON();
  void settingsEvent_Load(__attribute__((unused)) char *Url);                                            ///< Gets called when the /Settings.html is loaded. This page is for configuring the Gbox420 module settings (Console logging, Debug mode, MQTT reporting topic, Google Sheets relay...etc)
  void settingsEvent_Refresh(__attribute__((unused)) char *Url);                                         ///< Gets called when the /Settings.html is refreshed.
  void settingsEvent_Command(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data); ///< Gets called a button is clicked or a field is submitted on the /Settings.html page
  char *eventLogToJSON(bool IncludeKey = false, bool ClearBuffer = true);                                ///< Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"] and loads it to LongMessage buffer
  void addPushingBoxLogRelayID();                                                                        ///< Google Sheets reporting - Set PushingBox relay ID
  void relayToGoogleSheets(char (*JSONData)[MaxLongTextLength]);                                         ///< Google Sheets reporting - Send a JSON formatted report via REST API to the PushingBox relay
  void reportToGoogleSheetsTrigger(bool ForceRun = false);                                               ///< Google Sheets reporting - Handles custom reporting frequencies
  void mqttPublish(char (*JSONData)[MaxLongTextLength]);                                                 ///< MQTT reporting - Send a JSON formatted report to an MQTT broker
  void reportToMQTTTrigger(bool ForceRun = false);                                                       ///< MQTT reporting - Handles custom reporting frequencies

private:
protected:
  void setDebug(bool State);
  void setMetric(bool MetricEnabled);
  void setSerialReportingFrequency(uint16_t Frequency); ///< Set the of Frequency of Serial reports in seconds (5 sec increments, 5 sec minimum)
  void setSerialReportDate(bool State);                 ///< Enable/disable reporting the current time to the Serial output
  void setSerialReportMemory(bool State);               ///< Enable/disable reporting the remaining free memory to the Serial output
  void setSerialReportJSON(bool State);                 ///< Enable/disable sending JSON formatted reports to the Serial output
  void setSerialReportJSONFriendly(bool State);         ///< Enable/disable sending JSON report with friendly values (Sec,%,Min,kg/lbs..etc appended) to Serial
  void setSerialReportWireless(bool State);             ///< Enable/disable sending wireless package exchange reports to the Serial output
  bool RunRequested = false;
  bool ConsoleReportRequested = false;
  Common *ReportQueue[QueueDepth] = {}; ///< aggregate initializer: Same as initializing to null pointers
  Common *RefreshQueue[QueueDepth] = {};
  Common *CommandQueue[QueueDepth] = {};
  uint8_t ReportQueueItemCount = 0; ///< Tracking queue item count
  uint8_t RefreshQueueItemCount = 0;
  uint8_t CommandQueueItemCount = 0;

  char *getDebugText(bool FriendlyFormat = false);
  char *getMetricText(bool FriendlyFormat = false);
  void setSheetsReportingOnOff(bool State);
  void setSheetsReportingFrequency(uint16_t Frequency);
  void setPushingBoxLogRelayID(const char *ID);
  Common *WebsiteQueue_Load[QueueDepth] = {};
  Common *WebsiteQueue_Refresh[QueueDepth] = {};
  uint8_t WebsiteQueue_Load_Count = 0;
  uint8_t WebsiteQueue_Refresh_Count = 0;
  bool RefreshRequested = false;
  bool ReportToGoogleSheetsRequested = false;
  bool MQTTReportRequested = false;
  bool *ReportToGoogleSheets;
  uint16_t *SheetsReportingFrequency;
  uint8_t SheetsTriggerCounter = 0;
  bool *ReportToMQTT;
  uint16_t *MQTTReportFrequency;
  uint16_t MQTTTriggerCounter = 0;
};