#pragma once

/**
 * @file ReservoirModule.h
 * @author GrowBoxGuy (growboxguy@gmail.com)
 * @brief Module for monitoring a nutrient reservoir. Measures PH, Water/Air temperature, humidity and the water level 
 * @version 4.20
 * 
 * Responsible for setting up the components attached to the Reservoir module (Temperature/Weight/pH sensors..), 
 * and reporting the sensor readings to the Main module wirelessly.
 * 
 * @copyright https://sites.google.com/site/growboxguy
 * 
 */

#include "RF24.h"       ///https://github.com/maniacbug/RF24
#include "../Components/420Common.h"
#include "../Components/420Module.h"
#include "../WirelessCommands_Reservoir.h"

///forward declaration
class Sound;
class DHTSensor;
class PHSensor;
class WaterTempSensor;
class WeightSensor;
extern RF24 Wireless;

class ReservoirModule : virtual public Common, virtual public Module
{
public:
  //// @param Name Pointer to the Name of the object, stored in Flash
  //// @param DefaultSettings A pointer to the default settings stuct loaded from EEPROM. Defined in Settings.h

  ReservoirModule(const __FlashStringHelper *Name, Settings::ReservoirModuleSettings *DefaultSettings); ///constructor
  Sound *Sound1;        ///Pointer to a Piezo speaker - sound feedback          
  DHTSensor *DHT1;      ///Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  PHSensor *PHSen1;
  WaterTempSensor *WTemp1;  
  WeightSensor *Weight1;
  void refresh_Sec();
  void refresh_FiveSec();
  //void refresh_Minute();
  //void refresh_QuarterHour(); 
  void processCommand(void *Command);
  void updateResponse();
  void updateAckData();

private:
  unsigned long LastMessageReceived = 0;  //When was the last wireless message received

protected:
};