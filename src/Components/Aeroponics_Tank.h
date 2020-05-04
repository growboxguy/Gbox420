#pragma once

#include "420Common.h"
#include "Aeroponics.h"

///Aeroponics tote with pressure tank

class Aeroponics_Tank : public Aeroponics
{
public:
  Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor);
  void refresh_Sec();
  void report();
  void setPressureLow(float PressureLow);
  void setPressureHigh(float PressureHigh);
  void sprayNow(bool UserRequest = false);
  void sprayOff();

private:

protected:
  void checkRelays();
  char *sprayStateToText();
  void refillTank();
  uint8_t *SpraySolenoidPin;
  bool SpraySolenoidOn = false; ///Aeroponics - Controls the spray valve, set to true to spay at power on.
  float *PressureLow;           ///Aeroponics - Turn on pump below this pressure (bar)
  float *PressureHigh;          ///Aeroponics - Turn off pump above this pressure (bar)
};