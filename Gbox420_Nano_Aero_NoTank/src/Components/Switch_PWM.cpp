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

void Switch_PWM::report(bool JSONReport)
{
  Common::report(JSONReport); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  if (JSONReport)             //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    strcat_P(LongMessage, (PGM_P)F("\"DC\":\""));
    strcat(LongMessage, getDutyCycleText(false));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else //Print a report to the Serial console
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("DutyCycle:"));
    strcat(LongMessage, getDutyCycleText(true));
    logToSerials(&LongMessage, true, 1);
  }
}

void Switch_PWM::setDutyCycle(uint8_t DutyCycle)
{
  if (*this->DutyCycle != DutyCycle)
  {
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

char *Switch_PWM::getDutyCycleText(bool IncludeUnits)
{
  if (IncludeUnits)
  {
    return toText_percentage(*DutyCycle);
  }
  else
  {
    return toText(*DutyCycle);
  }
}