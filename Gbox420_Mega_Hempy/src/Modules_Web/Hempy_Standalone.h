#pragma once

///< This class represents a two bucket Hempy setup with common nutrient and waste reservoirs
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
  Hempy_Standalone(const __FlashStringHelper *Name, Settings::Hempy_StandaloneSettings *DefaultSettings);
  Sound *Sound1 = NULL;           ///< Pointer to a Piezo speaker - sound feedback
  DHTSensor *DHT1 = NULL;         ///< Humidity and temp sensor
  WeightSensor *WeightNR1 = NULL; ///< Nutrient reservoir weight sensor
  WeightSensor *WeightWR1 = NULL; ///< Waste reservoir weight sensor                                                                                                     ///< Pointer to a Digital Humidity and Temp Sensor object
  WeightSensor *WeightB1 = NULL;  ///< Bucket 1 reservoir weight sensor
  WeightSensor *WeightB2 = NULL;  ///< Bucket 2 reservoir weight sensor
  HempyBucket *Bucket1 = NULL;    ///< Hempy bucket 1
  HempyBucket *Bucket2 = NULL;    ///< Hempy bucket 2
  WaterPump *Pump1 = NULL;        ///< Bucket 1 water pump
  WaterPump *Pump2 = NULL;        ///< Bucket 2 water pump
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool FriendlyFormat = false);
  void refresh_FiveSec();
  void refresh_Minute();
  void reportToGoogleSheetsTrigger(bool ForceRun = false);
  void reportToMQTTTrigger(bool ForceRun = false);

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