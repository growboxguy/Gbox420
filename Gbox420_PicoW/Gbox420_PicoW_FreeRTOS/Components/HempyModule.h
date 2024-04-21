#pragma once

///< A two bucket Hempy setup with common nutrient and waste reservoirs
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

class HempyModule : public Module
{
public:
  HempyModule(Settings::HempyModuleSettings *DefaultSettings, Settings *GboxSettings);
  Sound *Sound1 = nullptr; ///< Pointer to a Piezo speaker - sound feedback
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