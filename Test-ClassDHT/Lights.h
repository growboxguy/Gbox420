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
    Lights(byte *Pin, byte* DimmingPin, byte* DimmingLimit, bool *Status, byte *Brightness, bool *TimerEnabled, byte *OnHour, byte *OnMinute, byte *OffHour, byte *OffMinute);  //constructor
    void refresh();  //Called when component should refresh its state
    void setBrightness(byte Brightness, bool AddToLog);     
    void triggerCalibrateLights();    //Website signals to calibrate the lights the next time the Light object gets CPU time       
    void setLightOnOff(bool State,bool AddToLog);    
    void setTimerOnOff(bool State);    
    void setOnHour(byte OnHour);    
    void setOnMinute(byte OnMinute);    
    void setOffHour(byte OffHour);    
    void setOffMinute(byte OffMinute);
  
  private:  
    bool CalibrateLights = false; //Stores the Calibration request
    void calibrateLights(); //Actual calibration code, takes to long to run directly from a website command  (Case sensitive object names! :) )
    void checkLightStatus();    
    void checkLightTimer();
     

    //light Sensor related - Need to split to separate class
    void checkLightSensor();
    bool PreviousLightRead = true;    
    int LightReading;  //light sensor analog reading
    bool Bright;  //Ligth sensor digital feedback: True-Bright / False-Dark 
    int LightsTriggerCount = 0;  //Counters of out of range readings before triggering an alert
    bool LightOK = true; //Track component health, at startup assume every component is OK
    int MaxLightReading = 0; // stores the highest light sensor analog reading
    int MinLightReading = 1023; //stores the lowest light sensor analog reading  
    
/*
void calibrateLights(){
  CalibrateLights=false;  
  bool LastStatus = *Status;
  byte LastLBrightness = *Brightness;
  setLightOnOff(true,false);
  checkLightStatus();  //apply turning the lights on
  setBrightness(0,false);
  delay(2000); //wait for light output change
  MinLightReading = 1023 - analogRead(LightSensorAnalogInPin);
  setBrightness(100,false);
  delay(2000); //wait for light output change
  MaxLightReading = 1023 - analogRead(LightSensorAnalogInPin);
  
  if(MySettings.DebugEnabled){
         logToSerials(F("0% - "),false); logToSerials(MinLightReading,false);
         logToSerials(F(", 100% - "),false); logToSerials(MaxLightReading,true);
  }
  setBrightness(LastLightBrightness,false); //restore original brightness
  MySettings.LightStatus=LastLightStatus; //restore original state
  checkLightStatus();
  //addToLog(F("Lights calibrated"));
}
*/

};

#endif
