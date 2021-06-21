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

class Hempy_Standalone : public Module_Web
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
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool FriendlyFormat = false);
  void refresh_FiveSec();
  void refresh_Minute();


private:

protected:
  
};