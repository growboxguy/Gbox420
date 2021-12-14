#pragma once

///< This class represents an autonomous module with an Arduino Nano for controlling an AC Motor 
///< Responsible for setting the components attached to the module (PWM speed controller and 4 relays for on/off and direction),
///< updating their statuses and reporting it to the Main module wirelessly
///< Listens and reacts to wireless commands from the main module

//WIRELESS DISBLED// #include "RF24.h" // https://github.com/maniacbug/RF24
#include "../Components/420Common.h"
#include "../Components/420Module.h"
#include "../WirelessCommands_ACMotor.h"

///< forward declaration

class Sound;
class ACMotor;
//WIRELESS DISBLED// extern RF24 Wireless;

class ACMotorModule : public Module
{
public:
  ///< @param Name Pointer to the Name of the object, stored in Flash
  ///< @param DefaultSettings A pointer to the default settings stuct loaded from EEPROM. Defined in Settings.h

  ACMotorModule(const __FlashStringHelper *Name, Settings::ACMotorModuleSettings *DefaultSettings); ///< constructor
  Sound *Sound1 = NULL;                                                                         ///< Pointer to a Piezo speaker - sound feedback
  ACMotor *Motor1 = NULL;  
  void report(__attribute__((unused)) bool FriendlyFormat = false){};
  void processTimeCriticalStuff();                ///< Process things that cannot wait or need precise timing
  void refresh_Sec();
  void refresh_FiveSec();
  //WIRELESS DISBLED// bool processCommand(void *Command);
  //WIRELESS DISBLED// void updateResponse();
  //WIRELESS DISBLED// void updateAckData(ACMotorMessages NewSequenceID);

private:
  //WIRELESS DISBLED// uint32_t LastMessageReceived = 0; //When was the last wireless message received

protected:
};