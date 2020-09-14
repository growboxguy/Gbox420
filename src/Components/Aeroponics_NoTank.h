#pragma once

#include "420Common.h"
#include "Aeroponics.h"

///Aeroponics tote without pressure tank

class Aeroponics_NoTank : public Aeroponics
{
public:
  Aeroponics_NoTank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump); ///constructor
  void refresh_Sec();
  void report();
  void sprayNow(bool UserRequest = false);
  void sprayOff();
  void lockPumpOn(); ///< On user request the pump can be turned ON without the spray timer turning it off.

private:

protected:
  bool LockedPumpOn = false; ///< While true only the Pump Timeout will stop the pump.
};