#pragma once

#include "420Common.h"
#include "420Module.h"

class Fan : public Common
{
public:
  Fan(const __FlashStringHelper *Name, Module *Parent, Settings::FanSettings *DefaultSettings);
  void refresh_Minute();
  void report();
  char *fanSpeedToText();
  char *fanSpeedToNumber();

private:  
  bool *State;
  bool *HighSpeed;
  byte *OnOffPin;
  byte *SpeedPin;
  void checkFanStatus();
  void TurnOff();
  void SetLowSpeed();
  void SetHighSpeed();

protected:
  Module *Parent;
};