#pragma once

///< Switch_PWM class: Represents an adjustable electronic switch based on a transistor
///< Warning: Mechanical relays cannot be used with PWM!!! Only Solid state relays.

#include "Switch.h"

class Switch_PWM : public Switch
{
public:
  Switch_PWM(const __FlashStringHelper *Name, uint8_t Pin, uint8_t *DutyCycle, uint8_t *DutyCycleLowLimit, bool NegativeLogic = false);
  void setDutyCycle(uint8_t DutyCycle);
  void turnOn();
  uint8_t getDutyCycle();
  char *getDutyCycleText();

private:
protected:
  uint8_t *DutyCycle = 0;         ///< PWM duty cycle when the switch is on. 100 = always on.
  uint8_t *DutyCycleLowLimit = 0; ///< Limits the lowers PWM duty cycle when the switch is on.
};