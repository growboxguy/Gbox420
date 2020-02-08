#pragma once //header guard, same as the ifndef/define guard  https://en.wikipedia.org/wiki/Pragma_once

//This is a virtual class from which all other classes inherit from (GrowBox, PH reader, Lights, DHTSensor..)
//Enforces a set of common functions shared by the components

#include "Arduino.h"     //every inheriting class have Arduino commands available
#include "TimeLib.h"     //keeping track of time
#include "../Settings.h" //for loading defaults and storing/reading user settings
#include "../Helpers.h"  //global functions


//forward declaration
extern Settings MySettings;
extern char LongMessage[MaxLongTextLength];
extern char ShortMessage[MaxShotTextLength];
extern char CurrentTime[MaxTextLength];
extern ELClientWebServer WebServer;
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
  virtual void websiteEvent_Load(__attribute__((unused)) char *url){}; //the "__attribute__((unused))" in front of the url parameter tells the compiler the variable is meant to be possibly unused. GCC will not produce an unused variable warning.
  virtual void websiteEvent_Refresh(__attribute__((unused)) char *url){};
  virtual void websiteEvent_Button(__attribute__((unused)) char *Button){};
  virtual void websiteEvent_Field(__attribute__((unused)) char *Field){};
  bool isThisMyComponent(char const *lookupName); //When it return true the component belongs to this object
  char *getWebsiteComponentName(const __FlashStringHelper *Name);

private:
protected:
  Common(const __FlashStringHelper *Name); //Constructor
  Common() {}                              //Constructor
};
