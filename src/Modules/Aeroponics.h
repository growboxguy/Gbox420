#pragma once

#include "../../420Common.h"
#include "Sound.h"
#include "PressureSensor.h"

class GrowBox;  //forward declaration

class Aeroponics : public Common
{
  public:
    Aeroponics(const __FlashStringHelper * Name, GrowBox * GBox, Settings::AeroponicsSettings * DefaultSettings, PressureSensor * FeedbackPressureSensor);
    virtual void websiteEvent_Load(__attribute__((unused)) char * url);
    virtual void websiteEvent_Refresh(__attribute__((unused)) char * url);
    virtual void websiteEvent_Button(__attribute__((unused)) char * Button);
    virtual void websiteEvent_Field(__attribute__((unused)) char * Field);
    virtual void report();
    //int AeroPressure = 6; //TODO: FAKE PRESSURE READING - REMOVE IT
    bool PumpOK = true; //Aeroponics - High pressure pump health  
    bool PumpOn = false; //Aeroponics - High pressure pump state
    uint32_t PumpTimer = millis();  //Aeroponics - Pump cycle timer
    uint32_t SprayTimer = millis();  //Aeroponics - Spray cycle timer - https://www.arduino.cc/reference/en/language/functions/time/millis/  
    uint32_t LastRefill= 0;  
    byte * BypassSolenoidPin;
    bool BypassSolenoidOn = false; //Aeroponics - Controls the bypass valve, true opens the solenoid    
    bool MixInProgress = false; //Aeroponics - Used to temporary suspend pump timer and keep the high pressure pump on. Do not change.
    bool* SprayEnabled;  //Enable/disable misting
    int* Interval; //Aeroponics - Spray every 15 minutes
    int* Duration; //Aeroponics - Spray time in seconds
    int* PumpTimeout;  // Aeroponics - Max pump run time in seconds (6 minutes), measue zero to max pressuretank refill time and adjust accordingly
    int* PrimingTime;  // Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.
    void checkRelays();
    void setPumpOn(bool UserRequest);
    void setPumpOff(bool UserRequest);
    void PumpDisable();
    void setPumpTimeout(int Timeout);
    void setPrimingTime(int Timing);
    char * pumpStateToText();
    virtual char * sprayStateToText() = 0; //pure virtual function, has no definition. Defined in child classes only
    void Mix();
    virtual void sprayNow(bool FromWebsite = false) = 0;
    virtual void sprayOff() = 0;
    void setSprayOnOff(bool State);
    void setInterval(int interval);
    void setDuration(int duration);
    char * getInterval();
    char * getDuration();

   private:

   protected:
    GrowBox * GBox;
    byte * PumpPin;
    PressureSensor * FeedbackPressureSensor; //Pressure sensor object that will monitor the spray pressure
};

