#pragma once ///< header guard, same as the ifndef/define guard  https://en.wikipedia.org/wiki/Pragma_once

///< This is a virtual class from which all other classes inherit from (GrowBox, PH reader, Lights, DHTSensor..)
///< Enforces a set of Common functions shared by the components

#include <stdio.h>
#include <cstring>
#include "hardware/watchdog.h" //Watchdog to auto-reboot in case of an error
#include "../Settings.h"       ///< for loading defaults and storing/reading user settings
#include "Helpers.h"           ///< global functions

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
  const char *Name;                        ///< Name of the instance
  bool isThisMine(char const *lookupName); ///< Checks if lookupName starts with the Name of the instance followed by _
  char *getName(char *AppendToEnd, bool UnderscoreSeparator = false);
  void appendName(bool ClearBuffer = true);                                                                               ///< Adds the component name to the ShortMessage Buffor. If Clear=true flush the cache before adding the name
  virtual void report(__attribute__((unused)) bool FriendlyFormat = false) = 0;                                           ///< Pure virtual function with body, must be implemented in child classes  https://www.learncpp.com/cpp-tutorial/pure-virtual-functions-abstract-base-classes-and-interface-classes/
  void run1sec();                                                                                                 ///< Called every second
  void run5sec();                                                                                                 ///< Called every 5 seconds
  void run1min();                                                                                                 ///< Called every minute
 void runAll();
  virtual bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data) { return false; }; ///< Called then an external command arrives (From MQTT or User Interface)
  virtual void websiteEvent_Load(__attribute__((unused)) char *Url){};                                                    ///< Called when an ESP-link webpage is loading. The name of the page is passed in the URL attribute (example: /Settings.html)
  virtual void websiteEvent_Refresh(__attribute__((unused)) char *Url){};                                                 ///< Called when an ESP-link webpage is refreshing (Automatic, every 5sec)

private:
protected:
  Common(const char *Name);   ///< Constructor
  uint8_t RefreshCounter = 0; ///< incremented every time refresh() is called.
  // Common();
};