#pragma once

///< This class represents a two bucket Hempy setup with common nutrient and waste reservoirs
///< Responsible for setting up each module, updating their statuses and reporting it

#include "420Module.h"
#include "MQTTClient.h"
// #include "ELClientRest.h" ///< ESP-link - REST API

// forward declaration of classes
class Sound;
class MqttClient;
/*
class DHTSensor;
class WeightSensor;
class WasteReservoir;
class WaterPump;
class HempyBucket;
*/

// extern ELClientRest PushingBoxRestAPI;

class Hempy_Standalone : public Module
{
public:
  Hempy_Standalone(const char *Name, Settings::Hempy_StandaloneSettings *DefaultSettings, Settings::MqttClientSettings *MqttSettings);
  MqttClient *MosquittoMqtt = NULL; ///< Pointer to MQTT handler
  Sound *Sound1 = NULL;          ///< Pointer to a Piezo speaker - sound feedback
  void websiteEvent_Load(__attribute__((unused)) char *Url);
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool FriendlyFormat = false);
  /*
  void run1sec();
  void run5sec();
  void run1min();
  */
 
private:
protected:
};