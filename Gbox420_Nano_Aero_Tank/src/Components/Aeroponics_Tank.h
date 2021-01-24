#pragma once

#include "420Common.h"
#include "Aeroponics.h"
#include "Switch.h"

enum class AeroTankStates
{
  DISABLED,
  IDLE,  
  SPRAYING,
  RELEASEPRESSURE,
  REFILLING,
  MIXING,
  DRAINING  
};

///< Aeroponics tote with pressure tank
class Aeroponics_Tank : virtual public Aeroponics
{
public:
  Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor, PressurePump *Pump);
  void refresh_Sec();
  void report();
  void updateState(AeroTankStates NewState);
  void sprayNow(bool UserRequest = false);  ///< Start spraying
  void sprayOff(bool UserRequest = false);  ///< Stop spraying
  void refillTank();  ///< Recharge the pressure tank
  void drainTank(){};  ///< Release pressure from the tank
  void processTimeCriticalStuff(); ///< Process things that cannot wait or need precise timing

private:
protected:
  AeroTankStates State = AeroTankStates::SPRAYING;
  Switch *SpraySwitch; //Relay or MOSFET controlling the spray solenoid
  uint16_t *SpraySolenoidClosingDelay;
};