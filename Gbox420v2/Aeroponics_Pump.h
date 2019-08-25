#pragma once

#include "420Common.h"
#include "Aeroponics.h"
#include "Sound.h"

class GrowBox;  //forward declaration

class Aeroponics_Pump : public Aeroponics
{
Aeroponics_Pump(GrowBox * GBox, byte BypassSolenoidPin, byte PumpPin, Settings::AeroponicsSettings * DefaultSettings);  //constructor
int BlowOffTime = 3; //TODO: Make it part of the settings
void refresh();
void checkAeroPumpAlerts_WithoutPressureTank();
void setAeroBlowOnOff(bool State);
void aeroSprayOff();
void aeroSprayNow(bool DueToHighPressure);
};