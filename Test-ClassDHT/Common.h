//This is a virtual class from which all other classes are derived (PH reader, Lights, DHTSensor..)
//Shares a set of common functions like convertig sensor readings to text, getting the current date, converting between metric and imperial systems
#ifndef Common_H
#define Common_H

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "420Settings.h"  //for storing/reading defaults
#include "420Pins.h"  //Load Arduino pin layout
#include "TimeLib.h" //Keeping track of time

class Common{
  friend class RollingAverage;
  
  protected:
    static char Message[];   //temp storage for assembling log messages, buffer for REST and MQTT API messages
    static char CurrentTime[]; //buffer for storing current time
  
  public:
    static Settings MySettings;
    virtual void refresh() = 0;  //every class needs to have an implementation of this method. Called when component should refresh its state (for example Reading temperature and humidity using the DHTsensor class)
       
    template <class logLine> //fuction template: logToSerials can take any parameter type (int,float,bool,char..) and print it to Arduino and ESP-link serial consoles.
    static void logToSerials (const logLine& ToPrint,bool breakLine) { 
      if(breakLine){Serial.println(ToPrint);Serial3.println(ToPrint);}
      else{Serial.print(ToPrint);Serial3.print(ToPrint);}
    }

    static void addToLog(const __FlashStringHelper* Text);
    static void addToLog(const char* Text);
  
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
    static char * timeToText(byte Hour, byte Minute);
    static char * tempToText(float Temp);
    static char * percentageToText(float Humidity);    
    static const __FlashStringHelper * onOffToText(bool Status);  
    static const __FlashStringHelper * yesNoToText(bool Status); 
    static const __FlashStringHelper * statusToText(bool Status);
    static const __FlashStringHelper * enabledToText(bool Status);  
};

class RollingAverage
{  
  private:
    long Sum = 0;
    int History[RollingAverageQueueDepth] = {0}; //array to store historical readings, 10 readings default
    byte Oldest = 0; //Points to the oldest reading      
   
  public:  
    int updateAverage(int LatestReading);
    float updateAverage(float LatestReading);
    int getAverageInt();
    float getAverageFloat();
};

#endif
