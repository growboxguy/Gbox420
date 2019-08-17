#ifndef GrowBox_H
#define GrowBox_H

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "TimeLib.h" //Keeping track of time
#include "420Settings.h"  //for storing/reading defaults
#include "420Helpers.h"  //global functions


class DHTSensor;  //forward declaration of classes
class LightSensor;
class Lights;
class Buzzer;

extern Settings BoxSettings;
extern char Message[512];
extern char CurrentTime[20];
extern template void logToSerials(const*,bool BreakLine);

class GrowBox
{
  //friend class RollingAverage;
  protected:

  
  public:
  GrowBox(Settings *BoxSettings); //constructor
  Settings * BoxSettings;
  Buzzer * Buzzer1; //Pointer to a Piezo Buzzer - sound feedback
  DHTSensor * InternalDHTSensor;  //Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  DHTSensor * ExternalDHTSensor; //Pointer to a Digital Humidity Sensor object measuring the external temperature of the grow box
  LightSensor * LightSensor1; //Pointer to a Light Sensor object measuring light intensity in the grow box
  Lights * Light1;  //Pointer to a Light assembly 
  void refresh();
  char * reportToSerials();
 
  void runSec(); //triger all threads at startup
  void runFiveSec(); //needs to run first to get sensor readings
  void runMinute();
  void runHalfHour(); 

  char* eventLogToJSON(bool Append); //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]  
  void addToLog(const __FlashStringHelper* Text);
  void addToLog(const char* Text);
  void setMetricSystemEnabled(bool MetricEnabled); 
  
  private: 
  
};

#endif
