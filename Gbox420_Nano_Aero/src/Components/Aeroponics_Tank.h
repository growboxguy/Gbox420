#pragma once

#include "420Common.h"
#include "Aeroponics.h"

///Aeroponics tote with pressure tank

class Aeroponics_Tank : public Aeroponics
{
public:
  Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump);
  void refresh_Sec();
  void report();
  void sprayNow(bool UserRequest = false);
  void sprayOff(bool UserRequest = false);
  void refillTank();

private:

protected:
  void setSpraySolenoidOn();
  void setSpraySolenoidOff();  
  uint8_t *SpraySolenoidPin;
  bool SpraySolenoidOn = false; ///Aeroponics - Controls the spray valve, set to true to spay at power on.
};