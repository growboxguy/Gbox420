#pragma once

#include "420Common.h"
#include "Aeroponics.h"

class Aeroponics_NoTank : public Aeroponics
{
public:
  Aeroponics_NoTank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_NoTankSpecific *NoTankSpecificSettings, PressureSensor *FeedbackPressureSensor); //constructor
  void refresh_Sec();
  void report();
  float LastSprayPressure = 0; //tracks the last average pressure during a spray cycle

private:
 
protected:
  void bypassOn();
  void bypassOff();
  void sprayNow(bool FromWebsite = false);
  void sprayOff();
  char *sprayStateToText();
  int *BlowOffTime;               //After spraying open the bypass valve for X seconds to release pressure in the system
  bool BlowOffInProgress = false; //Aeroponics - True while bypass valve is open during a pressure blow-off. Only used without the Pressure Tank option.
};