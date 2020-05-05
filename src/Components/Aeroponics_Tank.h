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
  void setLowPressure(float LowPressure);
  void sprayNow(bool UserRequest = false);
  void sprayOff(bool UserRequest = false);

private:

protected:
  void setSpraySolenoidOn();
  void setSpraySolenoidOff();
  void refillTank();
  uint8_t *SpraySolenoidPin;
  bool SpraySolenoidOn = false; ///Aeroponics - Controls the spray valve, set to true to spay at power on.
  float *LowPressure;           ///Aeroponics - Turn on pump below this pressure (bar)
};