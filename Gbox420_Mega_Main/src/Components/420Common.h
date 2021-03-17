#pragma once ///< header guard, same as the ifndef/define guard  https://en.wikipedia.org/wiki/Pragma_once

///< This is a virtual class from which all other classes inherit from (GrowBox, PH reader, Lights, DHTSensor..)
///< Enforces a set of Common functions shared by the components

#include "Arduino.h"
#include "TimeLib.h"         // Keeping track of time
#include "../../Settings.h"  ///< for loading defaults and storing/reading user settings
#include "../../SerialLog.h" ///< global functions
#include "../Helpers.h"      ///< global functions

///< forward declaration
extern char LongMessage[MaxLongTextLength];
extern char ShortMessage[MaxShotTextLength];
extern char CurrentTime[MaxWordLength];
extern Settings *ModuleSettings;
extern bool *Debug;
extern bool *Metric;

class Common
{
public:
  const __FlashStringHelper *Name;
  bool isThisMyComponent(char const *lookupName); ///< Checks whether a component belongs to the caller class or not
  char *getComponentName(const __FlashStringHelper *Name);
  virtual void report();
  virtual void refresh_Sec();
  virtual void refresh_FiveSec();
  virtual void refresh_Minute();
  virtual void reportToJSON();
  
private:
protected:
  Common(const __FlashStringHelper *Name); ///< Constructor
  Common();
};