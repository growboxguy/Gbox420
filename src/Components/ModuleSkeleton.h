#pragma once
///This is a template for creating new modules

///#include "3rdPartyModlue.h" ///Load any required 3rd party modules here
#include "420Common.h"
#include "420Module.h"

///Example module with test data

class ModuleSkeleton : virtual public Common
{
public:
  ModuleSkeleton(const __FlashStringHelper *Name, Module *Parent, Settings::ModuleSkeletonSettings *DefaultSettings); ///constructor
  void refresh_Sec();
  void refresh_FiveSec();
  void refresh_Minute();
  void refresh_QuarterHour();
  void report();

private:

protected:
  Module *Parent;
    ///Variables stored in memory, values reset at reboot
  bool RuntimeBool = false;
  int RuntimeInt = 420;
  float RuntimeFloat = 4.2f;
  char RuntimeString[MaxWordLength] = "HailMary";
  RollingAverage *RollingInt;
  RollingAverage *RollingFloat;

  ///Variables stored in EEPROM, values kept between reboots
  bool *PersistentBool;
  int *PersistentInt;
  float *PersistentFloat;
};