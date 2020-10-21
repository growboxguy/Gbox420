#include "Switch_PWM.h"

Switch_PWM::Switch_PWM(const __FlashStringHelper *Name, uint8_t Pin, uint8_t *DutyCycle, bool NegativeLogic) : Switch(Name, Pin,NegativeLogic)
{
  this->Name = Name;
  this->DutyCycle = DutyCycle;
  logToSerials(F("Switch_PWM object created"), true, 1);
}

void Switch_PWM::setDutyCycle(uint8_t DutyCycle)
{
  *this->DutyCycle = DutyCycle;
  logToSerials(F("PWM duty cycle:"), false, 1);
  logToSerials(getDutyCycleText(), true, 1);
}

void Switch_PWM::turnOn()
{
  State = true;

  if (NegativeLogic)
  {
    analogWrite(Pin, *DutyCycle); ///with negative logic Switch_PWMes
  }
  else
  {
    analogWrite(Pin, *DutyCycle); ///with positive logic Switch_PWMes
  }
}

uint8_t Switch_PWM::getDutyCycle()
{
  return *DutyCycle;
}

char *Switch_PWM::getDutyCycleText()
{
  return toText_percentage(*DutyCycle);
}