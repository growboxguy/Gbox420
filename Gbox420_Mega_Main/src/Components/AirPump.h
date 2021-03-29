#pragma once

#include "420Common.h"
#include "420Module.h"

///< Air Pump controller

class AirPump : virtual public Common
{
public:
  AirPump(const __FlashStringHelper *Name, Module *Parent, Settings::AirPumpSettings *DefaultSettings);
  void refresh_Minute();
  void report(bool FriendlyFormat = false);
  void TurnOn();
  void TurnOff();
  bool setState(bool NewState);
  bool getState();
  char *getStateText(bool FriendlyFormat = false);

private:
protected:
  Module *Parent;
  bool *State;
  uint8_t *Pin;
  void checkStatus();
};