#pragma once

////This class represents an autonomous module with an Arduino Nano, like the Aeroponics or Hempy bucket modules
////Responsible for setting the components attached to the module (Temp, weight, ph...sensors), 
////updating their statuses and reporting it to the Main module wirelessly
////Listens and reacts to wireless commands from the main module

#include "RF24.h"       ///https://github.com/maniacbug/RF24
#include "../Components/420Common.h"
#include "../Components/420Module.h"
#include "../WirelessCommands_Aero.h"

///forward declaration
class Sound;
class Aeroponics_NoTank;
class Aeroponics_Tank;
class PressureSensor;
class WaterPump;
extern RF24 Wireless;


class AeroModule : virtual public Common, virtual public Module
{
public:
  //// @param Name Pointer to the Name of the object, stored in Flash
  //// @param DefaultSettings A pointer to the default settings stuct loaded from EEPROM. Defined in Settings.h

  AeroModule(const __FlashStringHelper *Name, Settings::AeroModuleSettings *DefaultSettings); ///constructor
  Sound *Sound1;             ///Pointer to a Piezo speaker - sound feedback
  PressureSensor *Pres1;
  WaterPump *Pump1;
  Aeroponics_NoTank *AeroNT1; 
  Aeroponics_Tank *AeroT1;


  void refresh_Sec();
  void refresh_FiveSec();
  //void refresh_Minute();
  //void refresh_QuarterHour(); 
  void processCommand(aeroCommand *Command);
  void updateResponse();

private:
  struct aeroCommand Command;  //Commands sent to the external Module
  struct aeroResponse Response; //The response from the external Module will be stored here, represents the current status of the external Module

  
protected:

};