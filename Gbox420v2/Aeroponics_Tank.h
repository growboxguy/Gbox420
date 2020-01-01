#pragma once

#include "420Common.h"
#include "Aeroponics.h"
#include "Sound.h"

class GrowBox;  //forward declaration

class Aeroponics_Tank : public Aeroponics
{
  public:
    Aeroponics_Tank(const __FlashStringHelper * Name, GrowBox * GBox, Settings::AeroponicsSettings * DefaultSettings, Settings::AeroponicsSettings_TankSpecific * TankSpecificSettings);
    void websiteEvent_Field(char * Field);
    void refresh_Sec();
    void report();
    byte * SpraySolenoidPin;
    bool SpraySolenoidOn = false; //Aeroponics - Controls the spray valve, set to true to spay at power on.
    bool PreviousPressureRead = true;
    float * PressureLow; //Aeroponics - Turn on pump below this pressure (bar)
    float * PressureHigh; //Aeroponics - Turn off pump above this pressure (bar)
    bool * QuietEnabled;  //Quiet time when the pump should not run
    bool * RefillBeforeQuiet; //Refill before quiet time
    byte * QuietFromHour;  //Quiet time to block pump - hour
    byte * QuietFromMinute; //Quiet time to block pump - minute
    byte * QuietToHour; //Quiet time end - hour
    byte * QuietToMinute; //Quiet time end - minute  
    void checkRelays();
    void checkAeroPumpAlerts_WithPressureTank();
    bool checkQuietTime();
    void setQuietFromHour(int Hour);
    void setQuietFromMinute(int Minute);
    void setQuietToHour(int Hour);
    void setQuietToMinute(int Minute);
    void setQuietOnOff(bool State);
    void setQuietRefillOnOff(bool State); //TODO: Remove this
    void sprayOff();
    void sprayNow(bool DueToHighPressure);
    void setPressureLow(float PressureLow);
    void setPressureHigh(float PressureHigh);
  
  private:

  protected:
};
