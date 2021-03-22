#pragma once

///< This class represents an autonomous module with an Arduino Nano, like the Aeroponics or Hempy bucket modules
///< Responsible for setting the components attached to the module (Temp, weight, ph...sensors),
///< updating their statuses and reporting it to the Main module wirelessly
///< Listens and reacts to wireless commands from the main module

#include "RF24.h" // https://github.com/maniacbug/RF24
#include "../Components/420Common.h"
#include "../Components/420Module.h"
#include "../WirelessCommands_Hempy.h"

///< forward declaration

class Sound;
class WeightSensor;
class WaterPump;
class HempyBucket;
extern RF24 Wireless;

class HempyModule : virtual public Common, virtual public Module
{
public:
  ///< @param Name Pointer to the Name of the object, stored in Flash
  ///< @param DefaultSettings A pointer to the default settings stuct loaded from EEPROM. Defined in Settings.h

  HempyModule(const __FlashStringHelper *Name, Settings::HempyModuleSettings *DefaultSettings); ///< constructor
  Sound *Sound1 = NULL;                                                                                ///< Pointer to a Piezo speaker - sound feedback
  //DHTSensor *DHT1;          ///< Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  WeightSensor *WeightB1 = NULL;
  WeightSensor *WeightB2 = NULL;
  WeightSensor *WeightWR1 = NULL;
  WeightSensor *WeightWR2 = NULL;
  HempyBucket *Bucket1 = NULL;
  HempyBucket *Bucket2 = NULL;
  WaterPump *Pump1 = NULL;
  WaterPump *Pump2 = NULL;
  void report(){};
  void reportToJSON(){};
  void refresh_Sec();
  void refresh_FiveSec();
  //void refresh_Minute();
  bool processCommand(void *Command);
  void updateResponse();
  void updateAckData();

private:
  unsigned long LastMessageReceived = 0; //When was the last wireless message received

protected:
};