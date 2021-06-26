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

///< This class represents a complete Module_Web with all of its components
///< Responsible for setting up each module, updating their statuses and reporting it

extern ELClientWebServer WebServer;
extern ELClientRest PushingBoxRestAPI;
extern ELClientMqtt MqttAPI;
extern bool MqttConnected;
class Sound;

class Module_Web : virtual public Common_Web, virtual public Module
{
public:
  Module_Web(const __FlashStringHelper *Name);            ///< constructor
  void addToWebsiteQueue_Load(Common_Web *Subscriber);    ///< Subscribing to the Website load event: Calls the websiteEvent_Load() method
  void addToWebsiteQueue_Refresh(Common_Web *Subscriber); ///< Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  void addToCommandQueue(Common_Web *Subscriber);         ///< Subscribing to commands from external systems (MQTT, HTTP): Calls the commandEvent method
  void websiteLoadEventTrigger(char *Url);
  void websiteRefreshEventTrigger(char *Url);
  virtual void websiteEvent_Load(__attribute__((unused)) char *url);
  virtual void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void commandEventTrigger(char *command, char *data);
  void addToLog(const __FlashStringHelper *Text, uint8_t indent = 3);
  void addToLog(const char *Text, uint8_t indent = 3);
  char *eventLogToJSON(bool IncludeKey = false, bool ClearBuffer = true); ///< Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"] and loads it to LongMessage buffer
  void addPushingBoxLogRelayID();
  void relayToGoogleSheets(char (*JSONData)[MaxLongTextLength]);
  void mqttPublish(char (*JSONData)[MaxLongTextLength]);
  virtual bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void reportToGoogleSheetsTrigger(bool ForceRun = false);
  void reportToMQTTTrigger(bool ForceRun = false);

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