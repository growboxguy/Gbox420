#pragma once

#include "420Common.h"
#include "Aeroponics.h"
#include "Switch.h"

///Aeroponics tote with pressure tank

class Aeroponics_Tank : virtual public Aeroponics
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
  Switch *SpraySwitch; //Relay or MOSFET controlling the spray solenoid
};