#pragma once //header guard, same as the ifndef/define guard  https://en.wikipedia.org/wiki/Pragma_once

//This is a virtual class from which all other classes inherit from (GrowBox, PH reader, Lights, DHTSensor..)
//Enforces a set of Common functions shared by the components

#include "Arduino.h"     //every inheriting class have Arduino commands available
#include "TimeLib.h"     //keeping track of time
#include "../Settings.h" //for loading defaults and storing/reading user settings
#include "../Helpers.h"  //global functions


//forward declaration
extern char LongMessage[MaxLongTextLength];
extern char ShortMessage[MaxShotTextLength];
extern char CurrentTime[MaxTextLength];
extern Settings * BoxSettings;
extern bool *DebugEnabled;
extern bool *MetricSystemEnabled;

class Common
{
public:
  const __FlashStringHelper *Name;
  virtual void report();
  virtual void refresh_Sec();
  virtual void refresh_FiveSec();
  virtual void refresh_Minute();
  virtual void refresh_QuarterHour(); 
 
private:
protected:
  Common(const __FlashStringHelper *Name); //Constructor
  Common() {}                              //Constructor
};
