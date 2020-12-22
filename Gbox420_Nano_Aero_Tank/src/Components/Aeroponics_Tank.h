#pragma once

#include "420Common.h"
#include "Aeroponics.h"
#include "Switch.h"

///< Aeroponics tote with pressure tank

class Aeroponics_Tank : virtual public Aeroponics
{
public:
  Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump);
  void refresh_Sec();
  void report();
  void sprayNow(bool UserRequest = false);  ///< Start spraying
  void sprayOff(bool UserRequest = false);  ///< Stop spraying
  void refillTank();  ///< Recharge the pressure tank
  void checkPump();  ///< Turn the pump on or off based on the current state
  void checkSpray(bool OnlyTurnOff = false); ///< Turn the spray solenoid on or off based on the current state.
  void processTimeCriticalStuff(); ///< Process things that cannot wait or need precise timing

private:
protected:
  Switch *SpraySwitch; //Relay or MOSFET controlling the spray solenoid
  uint16_t *SpraySolenoidClosingDelay;
};