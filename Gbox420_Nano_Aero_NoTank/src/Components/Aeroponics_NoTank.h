#pragma once

#include "420Common.h"
#include "Aeroponics.h"

///< Aeroponics tote without pressure tank

class Aeroponics_NoTank : virtual public Aeroponics
{
public:
  Aeroponics_NoTank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, PressurePump *Pump); ///< constructor
  void refresh_Sec();
  void report();
  void sprayNow(bool UserRequest = false);
  void sprayOff();
  void checkPump(bool OnlyTurnOff = false);  ///< Turn the pump on or off based on the current state
  void lockPumpOn(); ///< On user request the pump can be turned ON without the spray timer turning it off.
  void processTimeCriticalStuff(); ///< Process things that cannot wait or need precise timing

private:
protected:
  bool RunTillTimeout = false; ///< While true only the Pump Timeout will stop the pump.
};