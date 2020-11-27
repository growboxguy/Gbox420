#pragma once

#include "420Common.h"
#include "420Module.h"

///Air Pump controller

class AirPump : virtual public Common
{
public:
  AirPump(const __FlashStringHelper *Name, Module *Parent, Settings::AirPumpSettings *DefaultSettings);
  void refresh_Minute();
  void report();
  void TurnOn();
  void TurnOff();
  bool getState();
  char *getStateToText();

private:
protected:
  Module *Parent;
  bool *State;
  uint8_t *Pin;
  void checkStatus();
};