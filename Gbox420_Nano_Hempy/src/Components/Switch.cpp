#include "Switch.h"

Switch::Switch(const __FlashStringHelper *Name, uint8_t Pin, bool NegativeLogic) : Common(Name)
{
  this->Pin = Pin;
  this->NegativeLogic = NegativeLogic;
  pinMode(Pin, OUTPUT);
  turnOff(); ///< initialize in OFF state
  logToSerials(F("Switch created"), true, 3);
}

void Switch::report(bool JSONReport)
{
  Common::report(JSONReport);
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  { ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
    strcat(LongMessage, getStateText());
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
  }
  else //Print a report to the Serial console
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("State:"));
    strcat(LongMessage, getStateText());
    logToSerials(&LongMessage, true, 1);
  }
}

Switch::Switch(const __FlashStringHelper *Name) : Common(Name)
{
}

void Switch::flip()
{
  if (State)
    turnOff();
  else
    turnOn();
}

void Switch::turnOn()
{
  State = true;

  if (NegativeLogic)
  {
    digitalWrite(Pin, LOW); ///< with negative logic switches
  }
  else
  {
    digitalWrite(Pin, HIGH); ///< with positive logic switches
  }
}

void Switch::turnOff()
{
  State = false;

  if (NegativeLogic)
  {
    digitalWrite(Pin, HIGH); ///< with negative logic switches
  }
  else
  {
    digitalWrite(Pin, LOW); ///< with positive logic switches
  }
}

bool Switch::getState()
{
  return State;
}

char *Switch::getStateText()
{
  return toText_onOff(State);
}