//This is a virtual class from which all other classes are derived (PH reader, Lights, DHTSensor..)
//Shares a set of common functions like convertig sensor readings to text, getting the current date, converting between metric and imperial systems
#ifndef Common_H
#define Common_H

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "Settings.h"  //for storing/reading defaults
#include "TimeLib.h" //Keeping track of time
#include "RollingAverage.h"


class Common{
  friend class RollingAverage;
  
  protected:
    static char Message[512];   //temp storage for assembling log messages, buffer for REST and MQTT API messages
    static char CurrentTime[20]; //buffer for storing current time
  
  public:
    static Settings MySettings;
    virtual void refresh() = 0;  //every class needs to have an implementation for this. Called when component should refresh its state (for example Reading temperature and humidity using the DHTsensor class)
  
    static char * getFormattedTime();
    
    static float convertBetweenTempUnits(float);
    static float convertBetweenPressureUnits(float);
    static void setMetricSystemEnabled(bool MetricEnabled);
  
    static char * toText(int); 
    static char * toText(long); 
    static char * toText(float);  //function overloading: Same named function, different parameter type
    //char * toText(uint_t);  
    static char * toText(int Number1, int Number2,const char * Separator); 
    static char * toText(float Number1, float Number2,const char * Separator); 
    static char * toPrecisionText(float Number);  
    static char * timeToText(int Hour, int Minute);
    static char * tempToText(float Temp);
    static char * percentageToText(float Humidity);
    
    static const __FlashStringHelper * onOffToText(bool Status);  
    static const __FlashStringHelper * yesNoToText(bool Status); 
    static const __FlashStringHelper * statusToText(bool Status);
    static const __FlashStringHelper * enabledToText(bool Status);  
};

#endif
