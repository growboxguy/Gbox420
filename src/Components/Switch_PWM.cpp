#include "Switch_PWM.h"

Switch_PWM::Switch_PWM(const __FlashStringHelper *Name, uint8_t Pin, uint8_t *DutyCycle, uint8_t *DutyCycleLowLimit, bool NegativeLogic) : Switch(Name)
{
  this->Pin = Pin;
  this->NegativeLogic = NegativeLogic;
  this->DutyCycle = DutyCycle;
  this->DutyCycleLowLimit = DutyCycleLowLimit;
  pinMode(Pin, OUTPUT);
  turnOff(); ///< initialize in OFF state
  logToSerials(F("Switch_PWM object created"), true, 2);
}

void Switch_PWM::setDutyCycle(uint8_t DutyCycle)
{
  if (*this->DutyCycle != DutyCycle)
  {
    *this->DutyCycle = DutyCycle;
    logToSerials(F("PWM duty cycle:"), false, 1);
    logToSerials(getDutyCycleText(), true, 1);
  }
}

void Switch_PWM::turnOn()
{
  State = true;

  if (NegativeLogic)
  {
    analogWrite(Pin, 255 - map(*DutyCycle, 0, 100, int(255 * (*DutyCycleLowLimit / 100.0f)), 255)); ///< mapping motor speed to duty cycle https:///< www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
  }
  else
  {
    analogWrite(Pin, map(*DutyCycle, 0, 100, int(255 * (*DutyCycleLowLimit / 100.0f)), 255));
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