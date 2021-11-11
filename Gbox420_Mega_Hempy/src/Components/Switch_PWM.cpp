#include "Switch_PWM.h"

Switch_PWM::Switch_PWM(const __FlashStringHelper *Name, uint8_t Pin, uint8_t *DutyCycle, uint8_t *DutyCycleLowLimit, bool NegativeLogic) : Switch(Name)
{
  this->Pin = Pin;
  this->NegativeLogic = NegativeLogic;
  this->DutyCycle = DutyCycle;
  this->DutyCycleLowLimit = DutyCycleLowLimit;
  pinMode(Pin, OUTPUT);
  turnOff(); ///< initialize in OFF state
  logToSerials(F("Switch_PWM ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void Switch_PWM::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"DC\":\""));
  strcat(LongMessage, getDutyCycleText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void Switch_PWM::setDutyCycle(uint8_t DutyCycle)
{
  if (*this->DutyCycle != DutyCycle)
  {
    if (DutyCycle > 100)
      DutyCycle = 100;
    *this->DutyCycle = DutyCycle;
  }
}

void Switch_PWM::turnOn()
{
  State = true;

  if (NegativeLogic)
  {
    analogWrite(Pin, 255 - map(*DutyCycle, 0, 100, int(255 * (*DutyCycleLowLimit / 100.0f)), 255)); ///< mapping motor speed to duty cycle https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
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

char *Switch_PWM::getDutyCycleText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_percentage(*DutyCycle);
  }
  else
  {
    return toText(*DutyCycle);
  }
}