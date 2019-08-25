#pragma once

#include "420Common.h"
#include "Sound.h"

class GrowBox;  //forward declaration

class Aeroponics : public Common
{
public:
    Aeroponics(GrowBox * GBox, byte BypassSolenoidPin, byte PumpPin, Settings::AeroponicsSettings * DefaultSettings);
    GrowBox * GBox;

    int AeroPressure = 6; //FAKE PRESSURE READING - REMOVE IT
    byte BypassSolenoidPin;
    byte PumpPin;
    uint32_t SprayTimer = millis();  //Aeroponics - Spray cycle timer - https://www.arduino.cc/reference/en/language/functions/time/millis/
    uint32_t PumpTimer = millis();  //Aeroponics - Pump cycle timer

    bool PumpOK = true; //Aeroponics - High pressure pump health
    bool SpraySolenoidOn = false; //Aeroponics - Controls the spray valve, set to true to spay at power on. Only used with the Pressure Tank option.
    bool BypassSolenoidOn = false; //Aeroponics - Controls the bypass valve, true opens the solenoid
    bool PumpOn = false; //Aeroponics - High pressure pump state
    bool BypassActive = false; //Aeroponics - Used to temporary suspend pump timers and keep the high pressure pump on. Do not change.
    bool BlowOffInProgress = false; //Aeroponics - True while bypass valve is open during a pressure blow-off. Only used without the Pressure Tank option.
   // float AeroPressure = 0.0;  //Aeroponics - Current pressure (bar)

    bool * SprayEnabled;  //Enable/disable misting
    int * Interval; //Aeroponics - Spray every 15 minutes
    int * Duration; //Aeroponics - Spray time in seconds
    float * PressureLow; //Aeroponics - Turn on pump below this pressure (bar)
    float * PressureHigh; //Aeroponics - Turn off pump above this pressure (bar)
    int * PumpTimeout;  // Aeroponics - Max pump run time in seconds (6 minutes), measue zero to max pressuretank refill time and adjust accordingly
    int * PrimingTime;  // Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.
    void checkRelays(){logToSerials(F("checkRelays METHOD NOT IMPLEMENTED"),true,0);};
    virtual void refresh(bool Interrupt);  //Aeroponics class cannot be instantiated, 
    virtual void report(bool Interrupt);
    void setPumpOn(bool CalledFromWebsite);
    void setPumpOff(bool CalledFromWebsite);
    void PumpDisable();
    void Mix();
    void setSprayOnOff(bool State);
    void setInterval(int interval);
    void setDuration(int duration);
    void setPressureLow(float PressureLow);
    void setPressureHigh(float PressureHigh);
    const __FlashStringHelper * pumpStateToText();
    uint32_t LastRefill= 0;
    void setAeroPumpTimeout(int Timeout);
    void setAeroPrimingTime(uint32_t Timing);
    void setAeroPressureTankOnOff(bool State);  //TODO: REMOVE THIS!
};

