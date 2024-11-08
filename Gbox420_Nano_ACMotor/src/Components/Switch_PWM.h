#pragma once

///< PWM controllable electronic switch like a MOSFET transistor

#include "Switch.h"

class Switch_PWM : public Switch
{
public:
  Switch_PWM(const __FlashStringHelper *Name, uint8_t Pin, uint8_t &DutyCycle, uint8_t &DutyCycleLowLimit, uint8_t &DutyCycleHighLimit, bool &NegativeLogic);
  void report(bool FriendlyFormat = false);
  void setDutyCycle(uint8_t DutyCycle);
  void turnOn();
  uint8_t getDutyCycle();
  char *getDutyCycleText(bool FriendlyFormat = false);

private:
protected:
  uint8_t &DutyCycle;         ///< PWM duty cycle when the switch is on. 100 = always on.
  uint8_t &DutyCycleLowLimit; ///< Limits the lowers PWM duty cycle when the switch is on.
  uint8_t &DutyCycleHighLimit; ///< Limits the lowers PWM duty cycle when the switch is on.
  
};