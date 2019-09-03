#pragma once

#include "420Common.h"
#include "Aeroponics.h"
#include "Sound.h"

class GrowBox;  //forward declaration

class Aeroponics_NoTank : public Aeroponics
{
public:
    Aeroponics_NoTank(const __FlashStringHelper * Name, GrowBox * GBox, byte BypassSolenoidPin, byte PumpPin, Settings::AeroponicsSettings * DefaultSettings);  //constructor
    int BlowOffTime = 3; //TODO: Make it part of the settings
    void refresh();
    void report();
    void checkAeroPumpAlerts_WithoutPressureTank();
    void setAeroBlowOnOff(bool State);
    void sprayOff();
    void sprayNow(bool DueToHighPressure);
    
};