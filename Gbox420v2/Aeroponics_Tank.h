#pragma once

#include "420Common.h"
#include "Aeroponics.h"
#include "Sound.h"

class GrowBox;  //forward declaration

class Aeroponics_Tank : public Aeroponics
{
    public:
    Aeroponics_Tank(GrowBox * GBox, byte SpraySolenoidPin, byte BypassSolenoidPin, byte PumpPin, Settings::AeroponicsSettings * DefaultSettings, Settings::AeroponicsSettings_TankSpecific * TankSpecificSettings);
   
    byte SpraySolenoidPin;
    bool PreviousPressureRead = true;

    byte * QuietFromHour;  //Quiet time to block pump - hour
    byte * QuietFromMinute; //Quiet time to block pump - minute
    byte * QuietToHour; //Quiet time end - hour
    byte * QuietToMinute; //Quiet time end - minute
    bool * QuietEnabled;  //Enable/disable quiet time then pump should not run
    bool * RefillBeforeQuiet; //Enable/disable refill before quiet time


    void refresh();
    void report();
    void checkAeroPumpAlerts_WithPressureTank();
    bool checkQuietTime();
    void setQuietFromHour(int Hour);
    void setQuietFromMinute(int Minute);
    void setQuietToHour(int Hour);
    void setQuietToMinute(int Minute);
    void setQuietOnOff(bool State);
    void setQuietRefillOnOff(bool State); //TODO: Remove this
    void aeroSprayOff();
    void aeroSprayNow(bool DueToHighPressure);

};
