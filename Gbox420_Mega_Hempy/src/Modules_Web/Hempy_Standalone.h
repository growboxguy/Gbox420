#pragma once

///< This class represents a complete Hempy_Standalone with all of its components
///< Responsible for setting up each module, updating their statuses and reporting it

#include "420Module_Web.h"
#include "ELClientRest.h" ///< ESP-link - REST API

// forward declaration of classes
class Sound;
class DHTSensor;
class WeightSensor;
class WaterPump;
class HempyBucket;

extern ELClientRest PushingBoxRestAPI;

///< Represents the complete box with lights,temp/humidity/ph/light sensors,power meter, etc..

class Hempy_Standalone : virtual public Common_Web, public Module_Web
{
public:
  Hempy_Standalone(const __FlashStringHelper *Name, Settings::Hempy_StandaloneSettings *DefaultSettings); ///< constructor
  Sound *Sound1;                                                                                                          ///< Pointer to a Piezo speaker - sound feedback
  DHTSensor *DHT1;                                                                                                        ///< Pointer to a Digital Humidity and Temp Sensor object 
  WeightSensor *WeightB1 = NULL;
  WeightSensor *WeightB2 = NULL;
  WeightSensor *WeightWR1 = NULL;
  HempyBucket *Bucket1 = NULL;
  HempyBucket *Bucket2 = NULL;
  WaterPump *Pump1 = NULL;
  WaterPump *Pump2 = NULL;
  void websiteEvent_Load(char *url);
  void websiteEvent_Refresh(char *url);
  void commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool FriendlyFormat = false);
  void refresh_FiveSec();
  void refresh_Minute();
  void reportToGoogleSheetsTrigger(bool ForceRun = false);
  void reportToMQTTTrigger(bool ForceRun = false);
  bool getDayMode(); ///< Returns true if the lights are on or daylight is detected
  char *getDayModeText(bool FriendlyFormat = false);

private:
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

protected:
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