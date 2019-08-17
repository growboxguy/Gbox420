//This is a virtual class from which all other classes are derived (PH reader, Lights, DHTSensor..)
//Shares a set of common functions like convertig sensor readings to text, getting the current date, converting between metric and imperial systems
#ifndef Common_H
#define Common_H

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "TimeLib.h" //Keeping track of time
#include "420Settings.h"  //for storing/reading defaults
#include "420Helpers.h"  //global functions

extern Settings MySettings;
extern char Message[512];
extern char CurrentTime[20];
extern template void logToSerials(const*,bool BreakLine);

class Common{
  friend class RollingAverage;

  private:
  
  protected:
  
  public:
    virtual void refresh() = 0;  //every class needs to have an implementation of this method. Called when component should refresh its state (for example Reading temperature and humidity using the DHTsensor class)
    static char* eventLogToJSON(bool Append); //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]  
    static void addToLog(const __FlashStringHelper* Text);
    static void addToLog(const char* Text);
    static void setMetricSystemEnabled(bool MetricEnabled);       
};

#endif
