#pragma once

#include "420Common.h"

class GrowBox;  //forward declaration

class Lights : public Common
{
  protected:
    GrowBox * GBox; //Pointer to the GrowBox object that contains the Lights object
    byte* RelayPin; //the Arduino pin controlling the AC relay
    byte* DimmingPin; //PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
    byte* DimmingLimit; //Sets the maximum LED dimming limit (Usually around 5%)
    bool* Status;   //Lights ON or OFF
    byte* Brightness; //Light intensity: 0 - 100 range for controlling led driver output  
    bool* TimerEnabled;  //Enable timer controlling lights: true - Timer enabled, false - Timer disabled   
    byte* OnHour;  //Light ON time - hour
    byte* OnMinute; //Light ON time - minute
    byte* OffHour;//Light OFF time - hour
    byte* OffMinute; //Light OFF time - minute 

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
    __FlashStringHelper * getTimerOnOffText(); 
    __FlashStringHelper* getStatusText(); 
    byte getBrightness(); 
    bool getStatus();
    char * getOnTimeText();    
    char * getOffTimeText();
    void checkLightStatus();
   
  
  private:  
    bool CalibrateLights = false; //Stores the Calibration request
    void calibrateLights(); //Actual calibration code, takes to long to run directly from a website command  (Case sensitive object names! :) )
        
    void checkLightTimer(); 

};
