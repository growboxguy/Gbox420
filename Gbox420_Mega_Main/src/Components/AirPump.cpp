#include "AirPump.h"
#include "Sound.h"

///< AirPump controller (2 speed)

AirPump::AirPump(const __FlashStringHelper *Name, Module *Parent, Settings::AirPumpSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Pin = &DefaultSettings->Pin;
  State = &DefaultSettings->State;
  pinMode(*Pin, OUTPUT);
  digitalWrite(*Pin, HIGH); ///< Turn relay off initially
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("AirPump object created"), true, 3);
}

void AirPump::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("Status:"));
  strcat(LongMessage, getStateToText());
  logToSerials(&LongMessage, true, 1);
}

void AirPump::reportToJSON()
{
  Common::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, toText(getState()));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void AirPump::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  checkStatus();
}

void AirPump::checkStatus()
{
  if (*State) ///< True turns relay ON (LOW signal activates the Relay)
    digitalWrite(*Pin, LOW);
  else
    digitalWrite(*Pin, HIGH);
}

void AirPump::TurnOff()
{
  setState(false);
}

void AirPump::TurnOn()
{
  setState(true);
}

bool AirPump::setState(bool NewState)
{
  *State = NewState;
  appendName(true);
  if (*State)
  {
    strcat_P(ShortMessage, (PGM_P)F("ON"));
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    strcat_P(ShortMessage, (PGM_P)F("OFF"));
    Parent->getSoundObject()->playOffSound();
  }
  Parent->addToLog(ShortMessage);
}

bool AirPump::getState()
{
  return *State;
}

char *AirPump::getStateToText()
{
  return toText_onOff(*State);
}