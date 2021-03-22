#include "Switch_PWM.h"

Switch_PWM::Switch_PWM(const __FlashStringHelper *Name, uint8_t Pin, uint8_t *DutyCycle, uint8_t *DutyCycleLowLimit, bool NegativeLogic) : Switch(Name)
{
  this->Pin = Pin;
  this->NegativeLogic = NegativeLogic;
  this->DutyCycle = DutyCycle;
  this->DutyCycleLowLimit = DutyCycleLowLimit;
  pinMode(Pin, OUTPUT);
  turnOff(); ///< initialize in OFF state
  logToSerials(F("Switch_PWM object created"), true, 3);
}

void Switch_PWM::report(bool JSONReport)
{
  Common::report(JSONReport);
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  { ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"DC\":\""));
    strcat(LongMessage, getDutyCycleText());
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
  }
  else //Print a report to the Serial console
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("DutyCycle:"));
    strcat(LongMessage, getDutyCycleText());
    logToSerials(&LongMessage, true, 1);
  }
}

void Switch_PWM::setDutyCycle(uint8_t DutyCycle)
{
  if (*this->DutyCycle != DutyCycle)
  {
    *this->DutyCycle = DutyCycle;
    appendName(true);
    strcat_P(ShortMessage, (PGM_P)F("duty cycle updated"));
    logToSerials(&ShortMessage);
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

char *Switch_PWM::getDutyCycleText()
{
  return toText_percentage(*DutyCycle);
}