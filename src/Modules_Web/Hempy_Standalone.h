#pragma once

///< This class represents a two bucket Hempy setup with common nutrient and waste reservoirs
///< Responsible for setting up each module, updating their statuses and reporting it

#include "420Module_Web.h"
#include "ELClientRest.h" ///< ESP-link - REST API

// forward declaration of classes
class Sound_Web;
class DHTSensor_Web;
class WeightSensor_Web;
class WasteReservoir;
class WaterPump_Web;
class HempyBucket_Web;

extern ELClientRest PushingBoxRestAPI;

class Hempy_Standalone : public Module_Web
{
public:
  Hempy_Standalone(const __FlashStringHelper *Name, Settings::Hempy_StandaloneSettings *DefaultSettings);
  Sound_Web *Sound1 = NULL;        ///< Pointer to a Piezo speaker - sound feedback
  DHTSensor_Web *DHT1 = NULL;      ///< Humidity and temp sensor
  WeightSensor_Web *NRW = NULL;    ///< Nutrient reservoir weight sensor
  WeightSensor_Web *WRW = NULL;    ///< Waste reservoir weight sensor                                                                                                     ///< Pointer to a Digital Humidity and Temp Sensor object
  WeightSensor_Web *B1W = NULL;    ///< Bucket 1 reservoir weight sensor
  WeightSensor_Web *B2W = NULL;    ///< Bucket 2 reservoir weight sensor
  WaterPump_Web *B1P = NULL;       ///< Bucket 1 water pump
  WaterPump_Web *B2P = NULL;       ///< Bucket 2 water pump
  WasteReservoir *WR1 = NULL;      ///< Waste reservoir
  HempyBucket_Web *Bucket1 = NULL; ///< Hempy bucket 1
  HempyBucket_Web *Bucket2 = NULL; ///< Hempy bucket 2
  void websiteEvent_Load(__attribute__((unused)) char *Url);
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);  
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool FriendlyFormat = false);
  void refresh_FiveSec();
  void refresh_Minute();

private:
protected:
};