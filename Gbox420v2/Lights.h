#pragma once
//Defines 2 classes: Lights and LightSensor

#include "420Common.h"

class GrowBox;  //forward declaration

class Lights : public Common
{
  protected:
    GrowBox * GBox; //Pointer to the GrowBox object that contains the Lights object

  public:
    Lights(GrowBox * Gbox,byte RelayPin, byte DimmingPin, byte* DimmingLimit, bool *Status, byte *Brightness, bool *TimerEnabled, byte *OnHour, byte *OnMinute, byte *OffHour, byte *OffMinute);  //constructor
    void refresh();  //Called when component should refresh its state
    void report();
    void setBrightness(byte Brightness, bool AddToLog);           
    void setLightOnOff(bool State,bool AddToLog);    
    void setTimerOnOff(bool State);    
    void setOnHour(byte OnHour);    
    void setOnMinute(byte OnMinute);    
    void setOffHour(byte OffHour);    
    void setOffMinute(byte OffMinute);
    const __FlashStringHelper * getTimerOnOffText(); 
    const __FlashStringHelper * getStatusText(); 
    byte getBrightness(); 
    bool getStatus();
    char * getOnTimeText();    
    char * getOffTimeText();
    void checkLightStatus();
   
  
  private:  
    bool CalibrateLights = false; //Stores the Calibration request
    void calibrateLights(); //Actual calibration code, takes to long to run directly from a website command  (Case sensitive object names! :) )
    byte RelayPin; //the Arduino pin controlling the AC relay
    byte DimmingPin; //PWM based dimming, connected to optocoupler`s base over 1k ohm resistor    
    void checkLightTimer(); 
    byte* DimmingLimit; //Sets the maximum LED dimming limit (Usually around 5%)
    bool* Status;   //Lights ON or OFF
    byte* Brightness; //Light intensity: 0 - 100 range for controlling led driver output  
    bool* TimerEnabled;  //Enable timer controlling lights: true - Timer enabled, false - Timer disabled   
    byte* OnHour;  //Light ON time - hour
    byte* OnMinute; //Light ON time - minute
    byte* OffHour;//Light OFF time - hour
    byte* OffMinute; //Light OFF time - minute 

};

class LightSensor : public Common
{
  protected:

  public:
    LightSensor(GrowBox * GBox, byte DigitalPin, byte AnalogPin);
    GrowBox * GBox;
    void refresh();  //Called when component should refresh its state
    void report();
    bool getIsDark(); //Ligth sensor digital feedback: True(Bright) or False(Dark)
    int getReading(); //Light sensor analog feedback: 0(darkest) to 1023 (brightest)
    char * getReadingPercentage(); //Light sensor analog feedback relative to calibration values: 0 to 100%  //CALIBRATION NOT IMPLEMENTED
    const __FlashStringHelper * getIsDarkText();  
    char * getReadingText();
    void triggerCalibration();    //Website signals to calibrate the MAX/MIN readings the next time the object gets CPU time
    
  private: 
    byte DigitalPin; //D0 - LM393 light sensor digital in
    byte AnalogPin; //A0 - LM393 light sensor analog in
    bool IsDark;   
    RollingAverage * LightReading;  //keeps an average of previous readings: Smoothens sensor readings
    bool CalibrateRequested;
    void calibrate();
    int MaxReading; // stores the highest light sensor analog reading measured during calibration
    int MinReading; //stores the lowest light sensor analog reading measured during calibration
};
