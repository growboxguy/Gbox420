#pragma once

#include "420Common.h"
#include "420Module.h"
#include "Sound.h"

///WaterWaterPump feedback buzzer

class WaterPump : virtual public Common
{
public:
  WaterPump(const __FlashStringHelper *Name, Module *Parent, Settings::WaterPumpSettings *DefaultSettings); 
  void refresh_Sec();
  void report();
  void turnOn(bool ResetStatus = false);  ///Turn the pump on
  void turnOff(bool ResetStatus = false);  ///Turn the pump off
  void disable();  ///Disable the pump
  void checkRelay(); ///Flit the relay into the correct status ON/OFF based on PumpOn variable
  char * getState();  ///Text representation of the current state: ON/OFF/DISABLED
  bool getOnState();  ///Pump ON(1)/OFF(0)
  bool getEnabledState();  ///Pump ENABLED(1)/DISABLED(0) 
  int *Timeout;  ///Max pump run time in seconds, if the target weight is not reached within this time consider the pump broken

private:
  
 
protected:
  Module *Parent;
  byte *Pin;
  bool *PumpEnabled;  ///false= Block running the pump
  bool PumpOn = false;  ///true turns the pump on
  
  uint32_t PumpTimer = millis();  ///Used for Pump runtime tracking
};