#include "AirPump.h"
#include "Sound.h"

///AirPump controller (2 speed)

AirPump::AirPump(const __FlashStringHelper *Name, Module *Parent, Settings::AirPumpSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Pin = &DefaultSettings->Pin;
  State = &DefaultSettings->State;
  pinMode(*Pin, OUTPUT);
  digitalWrite(*Pin, HIGH); ///Turn relay off initially
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("AirPump object created"), true, 1);
}

void AirPump::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Status:"));
  strcat(LongMessage, getStateToText());
  logToSerials(&LongMessage, true, 1);
}

void AirPump::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  checkStatus();
}

void AirPump::checkStatus()
{
  if (*State) ///True turns relay ON (LOW signal activates the Relay)
    digitalWrite(*Pin, LOW);
  else
    digitalWrite(*Pin, HIGH);
}

void AirPump::TurnOff()
{
  *State = false;
  checkStatus();
  Parent->addToLog(F("AirPump OFF"));
  Parent->getSoundObject()->playOffSound();
}

void AirPump::TurnOn()
{
  *State = true;
  checkStatus();
  Parent->addToLog(F("AirPump ON"));
  Parent->getSoundObject()->playOnSound();
}

bool AirPump::getState()
{
  return *State;
}

char *AirPump::getStateToText()
{
  return toText_onOff(*State);
}