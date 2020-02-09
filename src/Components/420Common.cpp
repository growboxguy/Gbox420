#include "420Common.h"

Common::Common(const __FlashStringHelper *Name)
{
  this->Name = Name;
  logToSerials(F("Initializing "), false, 2);
  logToSerials(this->Name, false, 0);
  logToSerials(F("..."), false, 0); //Prints "Initializing NAME..." to the console
}

void Common::report()
{
  logToSerials(Name, false, 4);
  logToSerials(F("-"), false, 1); //Prints "    COMPONENTNAME -" to the console
}

void Common::refresh_Sec()
{
  logToSerials(Name, false, 2);
  logToSerials(F("refreshing (1sec)"), true, 1); //Prints "COMPONENTNAME refreshing (1 sec)" to the console
}

void Common::refresh_FiveSec()
{
  logToSerials(Name, false, 2);
  logToSerials(F("refreshing (5sec)"), true, 1); //Prints "COMPONENTNAME refreshing (5 sec)" to the console
}

void Common::refresh_Minute()
{
  logToSerials(Name, false, 2);
  logToSerials(F("refreshing (1min)"), true, 1); //Prints "COMPONENTNAME refreshing (1 min)" to the console
}

void Common::refresh_QuarterHour()
{
  logToSerials(Name, false, 2);
  logToSerials(F("refreshing (15min)"), true, 1); //Prints "COMPONENTNAME refreshing (15 min)" to the console
}