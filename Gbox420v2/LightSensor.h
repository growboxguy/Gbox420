#ifndef LightSensor_H
#define LightSensor_H

#include "Common.h"

class LightSensor : public Common
{
  protected:

  public:
    LightSensor(byte DigitalPin, byte AnalogPin);
    void refresh();  //Called when component should refresh its state
    bool getIsDark(); //Ligth sensor digital feedback: True(Bright) or False(Dark)
    int getReading(); //Light sensor analog feedback: 0(darkest) to 1023 (brightest)
    int getReadingPercentage(); //Light sensor analog feedback relative to calibration values: 0 to 100%  //CALIBRATION NOT IMPLEMENTED
    const __FlashStringHelper * getIsDarkText();  
    char * getReadingText();
    
  private: 
    byte DigitalPin; //D0 - LM393 light sensor digital in
    byte AnalogPin; //A0 - LM393 light sensor analog in
    bool IsDark;   
    RollingAverage * LightReading;  //keeps an average of previous readings: Smoothens sensor readings
};

#endif
