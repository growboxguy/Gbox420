#pragma once

///< PWM controllable electronic switch like a MOSFET transistor

#include "Switch.h"

class Switch_PWM : public Switch
{
public:
  Switch_PWM(const __FlashStringHelper *Name, uint8_t Pin, uint8_t *DutyCycle, uint8_t *DutyCycleLowLimit, bool NegativeLogic = false);
  void report(bool JSONReport = false);
  void setDutyCycle(uint8_t DutyCycle);
  void turnOn();
  uint8_t getDutyCycle();
  char *getDutyCycleText();

private:
protected:
  uint8_t *DutyCycle = 0;         ///< PWM duty cycle when the switch is on. 100 = always on.
  uint8_t *DutyCycleLowLimit = 0; ///< Limits the lowers PWM duty cycle when the switch is on.
};