#include "Switch.h"

Switch::Switch(const __FlashStringHelper *Name, uint8_t Pin, bool NegativeLogic, uint16_t FlipDelay) : Common(Name)
{
  this->Pin = Pin;
  this->NegativeLogic = NegativeLogic;
  this->FlipDelay = FlipDelay;
  pinMode(Pin, OUTPUT);
  turnOff(); ///< initialize in OFF state
  logToSerials(F("Switch ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void Switch::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, getStateText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
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

void Switch::turnOn_Wait()
{
  turnOn();
  delay(FlipDelay);
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

void Switch::turnOff_Wait()
{
  turnOff();
  delay(FlipDelay);
}

bool Switch::getState()
{
  return State;
}

char *Switch::getStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_onOff(State);
  }
  else
  {
    return toText(State);
  }
}