#pragma once

////This class represents an autonomous module with an Arduino Nano
////Responsible for setting the components attached to the module (Temp, weight, ph...sensors), 
////updating their statuses and reporting it to the Main module wirelessly
////Listens and reacts to wireless commands from the main module

#include "RF24.h"       ///https://github.com/maniacbug/RF24
#include "../Components/420Common.h"
#include "../Components/420Module.h"
#include "../WirelessCommands_Reservoir.h"

///forward declaration
class Sound;
class DHTSensor;
class PHSensor;
class WaterLevelSensor;
class WaterTempSensor;
class DistanceSensor;
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
  WaterLevelSensor *WLev1;
  WaterTempSensor *WTmp1;  
  DistanceSensor *Dist1;
  //void refresh_Sec();
  void refresh_FiveSec();
  //void refresh_Minute();
  //void refresh_QuarterHour(); 
  void processCommand(reservoirCommand *Command);
  void updateResponse();

private:
  struct reservoirCommand Command;  //Commands sent to the external Module
  struct reservoirResponse Response; //The response from the external Module will be stored here, represents the current status of the external Module
  
protected:
};