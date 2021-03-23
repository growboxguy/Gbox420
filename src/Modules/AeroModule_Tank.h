#pragma once

///< This class represents an autonomous module with an Arduino Nano, like the Aeroponics or Hempy bucket modules
///< Responsible for setting the components attached to the module (Temp, weight, ph...sensors),
///< updating their statuses and reporting it to the Main module wirelessly
///< Listens and reacts to wireless commands from the main module

#include "RF24.h" // https://github.com/maniacbug/RF24
#include "../Components/420Common.h"
#include "../Components/420Module.h"
#include "../WirelessCommands_Aero.h"

///< forward declaration
class Sound;
class Aeroponics_Tank;
class PressureSensor;
class PressurePump;
class WeightSensor;
extern RF24 Wireless;

class AeroModule : virtual public Common, virtual public Module
{
public:
  ///< @param Name Pointer to the Name of the object, stored in Flash
  ///< @param DefaultSettings A pointer to the default settings stuct loaded from EEPROM. Defined in Settings.h

  AeroModule(const __FlashStringHelper *Name, Settings::AeroponicsModuleSettings *DefaultSettings); ///< constructor
  Sound *Sound1 = NULL;                        ///< Piezo speaker - sound feedback
  PressureSensor *Pres1 = NULL;  ///< Pressure sensor
  PressurePump *Pump1 = NULL; ///< High pressure pump
  Aeroponics_Tank *AeroT1 = NULL; ///< Logic to control the Aeroponics system with a pressure tank
  WeightSensor *Weight1; ///< Tote weight sensor
  void report(__attribute__((unused)) bool JSONReport = false){};
  void refresh_Sec();
  void refresh_FiveSec();
  //void refresh_Minute();
  void processTimeCriticalStuff(); ///< Process things that cannot wait or need precise timing
  bool processCommand(void *Command); ///< Process a received wireless command, returns true when the last message in the exchange sequence was processed 
  void updateResponse();  ///< Updates the wireless response package content
  void updateAckData();   ///< pre-loads the ACK buffer that gets sent back when receiving a wireless message

private:
  unsigned long LastMessageReceived = 0; ///< Timestamp of the last received wireless message 

protected:
};