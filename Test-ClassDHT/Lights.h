#ifndef Lights_H
#define Lights_H

#include "Common.h"

class Lights : public Common
{
  protected:
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
    Lights(byte RelayPin, byte DimmingPin, byte* DimmingLimit, bool *Status, byte *Brightness, bool *TimerEnabled, byte *OnHour, byte *OnMinute, byte *OffHour, byte *OffMinute);  //constructor
    void refresh();  //Called when component should refresh its state
    void setBrightness(byte Brightness, bool AddToLog);     
    void triggerCalibrateLights();    //Website signals to calibrate the lights the next time the Light object gets CPU time       
    void setLightOnOff(bool State,bool AddToLog);    
    void setTimerOnOff(bool State);    
    void setOnHour(byte OnHour);    
    void setOnMinute(byte OnMinute);    
    void setOffHour(byte OffHour);    
    void setOffMinute(byte OffMinute);
    __FlashStringHelper * getTimerOnOffText(); 
    __FlashStringHelper* getStatusText();   
    char * getOnTimeText();    
    char * getOffTimeText();
   
  
  private:  
    bool CalibrateLights = false; //Stores the Calibration request
    int MaxReading; // stores the highest light sensor analog reading
    int MinReading; //stores the lowest light sensor analog reading  
    void calibrateLights(); //Actual calibration code, takes to long to run directly from a website command  (Case sensitive object names! :) )
    void checkLightStatus();    
    void checkLightTimer(); 

};

#endif
