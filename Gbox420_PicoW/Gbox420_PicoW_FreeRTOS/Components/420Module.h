/*! \file
 *  \brief     Parent class of all Modules, provides a set of shared functions and variables to Modules
 *  \details   Functions to interact with MQTT/HTTP, report to stdout and schedule refresh triggers
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/
 *  \version   4.20
 */

#pragma once

/*
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
class MqttClient;
extern MqttClient *DefaultMqttClient;
extern void mqttPublish(const char *Topic, const char *Data);

class Module : virtual public Common
{
public:
  Module(const char *Name);                                                                                                           ///< constructor
  void reportToSerial(bool ForceRun = false, bool ClearBuffer = true, bool KeepBuffer = false, bool JSONOnly = false);                ///< Generate a text log of all sensor readings to the Serial output and/or to the LongMessage buffer.
  void run1sec();                                                                                                                     ///< Called every second
  void run5sec();                                                                                                                     ///< Called every 5 seconds
  void run1min();                                                                                                                     ///< Called every minute
  void run30min();                                                                                                                    ///< Called every 30 minutes
  void runAll();                                                                                                                      ///< Triggers all runs
  void addToLog(const char *Text, uint8_t Indent = 3);                                                                                ///< Add a Log entry that is displayed on the web interface
  void addToReportQueue(Common *Component);                                                                                           ///< Subscribing to the report queue: Calls the report() method
  void addToRefreshQueue_1sec(Common *Component);                                                                                     ///< Subscribing to the 1 second refresh queue: Calls the run1sec() method
  void addToRefreshQueue_5sec(Common *Component);                                                                                     ///< Subscribing to the 5 second refresh queue: Calls the run5sec() method
  void addToRefreshQueue_1min(Common *Component);                                                                                     ///< Subscribing to the 1 minute refresh queue: Calls the run1min() method
  void addToRefreshQueue_30min(Common *Component);                                                                                    ///< Subscribing to the 30 minute refresh queue: Calls the run30min() method
  void addToCommandQueue(Common *Component);                                                                                          ///< Subscribing to commands from external systems (MQTT, HTTP): Calls the commandEvent() method
  void commandEventTrigger(char *Command, char *Data);                                                                                ///< Notifies the subscribed components of an incoming command. Command: combination of the Name of the component and a command (like Pump1_On, Light1_Brightness). Data: Optional value, passed as a character array (can be parsed to int/float/boolean)
  void mqttDataReceived(char *Topic, char *Data);                                                                                     ///< MQTT data received from the Subscribed topic

  Sound *DefaultSound = nullptr;
  Sound *getSoundObject();
  uint16_t SerialTriggerCounter = 0; ///< Helps with timing when to send the Serial report out
  bool *SerialReportDate;            ///< Enable/disable reporting the current time to the Serial output
  bool SerialReportMemory;          ///< Enable/disable reporting the remaining free memory to the Serial output
  bool SerialReportJSONFriendly;    ///< Enable/disable sending Text formatted reports to the Serial output
  bool SerialReportJSON;            ///< Enable/disable sending JSON formatted reports to the Serial output
  bool SerialReportWireless;        ///< Enable/disable sending wireless package exchange reports to the Serial output

  /*
    void addToWebsiteQueue_Load(Common *Subscriber);                    ///< Calls the websiteEvent_Load() method of the Subscriber when an ESP-link website is loaded
    void addToWebsiteQueue_Refresh(Common *Subscriber);                 ///< Calls the websiteEvent_Refresh() method of the Subscriber when an ESP-link website is refreshing
    void websiteLoadEventTrigger(__attribute__((unused)) char *Url);    ///< Notifies the subscribed components of a Load event. Passes the URL of the custom webpage that was opened (/Hempy.html or /Settings.html )
    void websiteRefreshEventTrigger(__attribute__((unused)) char *Url); ///< Notifies the subscribed components of a Refresh event. Passes the URL of the custom webpage that was opened (/Hempy.html or /Settings.html )
    void settingsEvent_Load(__attribute__((unused)) char *Url);                                            ///< Gets called when the /Settings.html is loaded. This page is for configuring the Gbox420 module settings (Console logging, Debug mode, MQTT reporting topic, Google Sheets relay...etc)
    void settingsEvent_Refresh(__attribute__((unused)) char *Url);                                         ///< Gets called when the /Settings.html is refreshed.
    void settingsEvent_Command(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data); ///< Gets called a button is clicked or a field is submitted on the /Settings.html page
    */
  void refresh();
  char *settingsToJSON();
  char *eventLogToJSON(bool IncludeKey = false, bool ClearBuffer = true); ///< Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"] and loads it to LongMessage buffer
  /*
  void addPushingBoxLogRelayID();                                                                        ///< Google Sheets reporting - Set PushingBox relay ID
  void relayToGoogleSheets(char (*JSONData)[MaxLongTextLength]);                                         ///< Google Sheets reporting - Send a JSON formatted report via REST API to the PushingBox relay
  void reportToGoogleSheetsTrigger(bool ForceRun = false);                                               ///< Google Sheets reporting - Handles custom reporting frequencies
*/
  void reportToMqttTrigger(bool ForceRun = false); ///< MQTT reporting - Handles custom reporting frequencies

private:
protected:
  void setSerialReportingFrequency(uint16_t Frequency); ///< Set the of Frequency of Serial reports in seconds (5 sec increments, 5 sec minimum)
  void setSerialReportDate(bool State);                 ///< Enable/disable reporting the current time to the Serial output
  void setSerialReportMemory(bool State);               ///< Enable/disable reporting the remaining free memory to the Serial output
  void setSerialReportJSON(bool State);                 ///< Enable/disable sending JSON formatted reports to the Serial output
  void setSerialReportJSONFriendly(bool State);         ///< Enable/disable sending JSON report with friendly values (Sec,%,Min,kg/lbs..etc appended) to Serial
  void setSerialReportWireless(bool State);             ///< Enable/disable sending wireless package exchange reports to the Serial output
  bool RunAllRequested = false;
  bool ConsoleReportRequested = false;
  std::vector<Common *> ReportQueue; ///< aggregate initializer: Same as initializing to null pointers
  std::vector<Common *> RefreshQueue_1sec;
  std::vector<Common *> RefreshQueue_5sec;
  std::vector<Common *> RefreshQueue_1min;
  std::vector<Common *> RefreshQueue_30min;
  std::vector<Common *> CommandQueue;

  /*
  void setSheetsReportingOnOff(bool State);
  void setSheetsReportingFrequency(uint16_t Frequency);
  void setPushingBoxLogRelayID(const char *ID);
  */
  // Common *WebsiteQueue_Load[QueueDepth] = {};
  // Common *WebsiteQueue_Refresh[QueueDepth] = {};
  // uint8_t WebsiteQueue_Load_Count = 0;
  // uint8_t WebsiteQueue_Refresh_Count = 0;
  bool RefreshRequested = false;
  // bool ReportToGoogleSheetsRequested = false;
  bool MQTTReportRequested = false;
  // bool *ReportToGoogleSheets;
  // uint16_t *SheetsReportingFrequency;
  // uint8_t SheetsTriggerCounter = 0;
  bool *ReportToMqtt;
  uint16_t *MQTTReportFrequency;
  uint16_t MQTTTriggerCounter = 0;
};