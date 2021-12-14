#pragma once

///< PWM controllable electronic switch like a MOSFET transistor

#include "Switch.h"

class Switch_PWM : public Switch
{
public:
  Switch_PWM(const __FlashStringHelper *Name, uint8_t Pin, uint8_t *DutyCycle, uint8_t *DutyCycleLowLimit, uint8_t *DutyCycleHighLimi, bool NegativeLogic = false);
  void report(bool FriendlyFormat = false);
  void setDutyCycle(uint8_t DutyCycle);
  void turnOn();
  uint8_t getDutyCycle();
  char *getDutyCycleText(bool FriendlyFormat = false);

private:
protected:
  uint8_t *DutyCycle = NULL;         ///< PWM duty cycle when the switch is on. 100 = always on.
  uint8_t *DutyCycleLowLimit = NULL; ///< Limits the lowers PWM duty cycle when the switch is on.
  uint8_t *DutyCycleHighLimit = NULL; ///< Limits the lowers PWM duty cycle when the switch is on.
  
};