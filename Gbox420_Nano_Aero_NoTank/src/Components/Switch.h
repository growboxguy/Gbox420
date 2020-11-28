#pragma once

///< Switch class: Represents and electronic switch like a Relay or MOSFET

#include "420Common.h"
#include "420Module.h"
#include "Sound.h"

class Switch : public Common
{
public:
  Switch(const __FlashStringHelper *Name, uint8_t Pin, bool NegativeLogic = false);
  Switch(const __FlashStringHelper *Name);
  void turnOn();
  void turnOff();
  void flip();          ///< invert the current state
  bool getState();      ///< Pump ON(1)/OFF(0)
  char *getStateText(); ///< Text representation of the current state: DISABLED/IDLE/RUNNING...

private:
protected:
  uint8_t Pin = 0;            ///< Switch pin
  bool State = false;         ///< true turns the Switch on
  bool NegativeLogic = false; ///< Use Negative logic where LOW signal turns on the Switch.
};