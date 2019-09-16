#pragma once

#include "420Common.h"
#include "Aeroponics.h"
#include "Sound.h"

class GrowBox;  //forward declaration

class Aeroponics_NoTank : public Aeroponics
{
public:
    Aeroponics_NoTank(const __FlashStringHelper * Name, GrowBox * GBox, byte BypassSolenoidPin, byte PumpPin, Settings::AeroponicsSettings * DefaultSettings, Settings::AeroponicsSettings_NoTankSpecific * NoTankSpecificSettings);  //constructor
    int * BlowOffTime; //TODO: Make it part of the settings
    bool BlowOffInProgress = false; //Aeroponics - True while bypass valve is open during a pressure blow-off. Only used without the Pressure Tank option.
    void refresh();
    void report();
    void websiteLoadEvent();
    void websiteFieldSubmitEvent(char * Field);
    void checkAlerts();
    void setBlowOffTime(int _BlowOffTime);
    void sprayOff();
    void sprayNow(bool DueToHighPressure);
    
};