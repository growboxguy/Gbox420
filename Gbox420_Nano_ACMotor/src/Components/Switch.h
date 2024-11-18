#pragma once

///< Represents and electronic switch like a Relay or MOSFET

#include "420Common.h"
#include "420Module.h"
#include "Sound.h"

class Switch : public Common
{
public:
  Switch(const __FlashStringHelper *Name, uint8_t Pin, bool NegativeLogic = false, uint16_t FlipDelay = 0);
  //Switch(const __FlashStringHelper *Name);
  void report(bool FriendlyFormat = false);
  void turnOn(); ///< Turn the switch on: Sends the signal, but does not wait for the switch to flip over physically
  void turnOn_Wait(); ///< Turn the switch on and wait for the switch to flip over physically. USES Delay() !!!
  void turnOff();  ///< Turn the switch off and wait for the switch to flip over physically. USES Delay() !!!
  void turnOff_Wait();  ///< Turn the switch off and wait for the switch to flip over physically. USES Delay() !!!
  void flip();                                     ///< invert the current state
  bool getState();                                 ///< Pump ON(1)/OFF(0)
  char *getStateText(bool FriendlyFormat = false); ///< Text representation of the current state: DISABLED/IDLE/RUNNING...

private:
protected:
  uint8_t Pin = 0;            ///< Switch pin
  bool State = false;         ///< true turns the Switch on
  bool NegativeLogic = false; ///< Use Negative logic where LOW signal turns on the Switch.
  uint16_t FlipDelay = 0; ///< How long it takes for the switch to flip
};