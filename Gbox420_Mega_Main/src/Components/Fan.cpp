#include "Fan.h"
#include "Sound.h"

///< Fan controller (2 speed)

Fan::Fan(const __FlashStringHelper *Name, Module *Parent, Settings::FanSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  OnOffPin = &DefaultSettings->OnOffPin;
  SpeedPin = &DefaultSettings->SpeedPin;
  State = &DefaultSettings->State;
  HighSpeed = &DefaultSettings->HighSpeed;
  pinMode(*OnOffPin, OUTPUT);
  digitalWrite(*OnOffPin, HIGH); ///< Turn relay off initially
  pinMode(*SpeedPin, OUTPUT);
  digitalWrite(*SpeedPin, HIGH); ///< Turn relay off initially
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("Fan object created"), true, 3);
}

void Fan::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  checkFanStatus();
}

void Fan::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("Status:"));
  strcat(LongMessage, fanSpeedToText());
  logToSerials(&LongMessage, true, 1);
}

void Fan::checkFanStatus()
{
  if (*State) ///< True turns relay ON (LOW signal activates the Relay)
    digitalWrite(*OnOffPin, LOW);
  else
    digitalWrite(*OnOffPin, HIGH);
  if (*HighSpeed)
    digitalWrite(*SpeedPin, LOW);
  else
    digitalWrite(*SpeedPin, HIGH);
}

void Fan::TurnOff()
{
  *State = false;
  *HighSpeed = false;
  checkFanStatus();
  Parent->addToLog(F("Fan OFF"));
  Parent->getSoundObject()->playOffSound();
}

void Fan::SetLowSpeed()
{
  *State = true;
  *HighSpeed = false;
  checkFanStatus();
  Parent->addToLog(F("Fan speed LOW"));
  Parent->getSoundObject()->playOnSound();
}

void Fan::SetHighSpeed()
{
  *State = true;
  *HighSpeed = true;
  checkFanStatus();
  Parent->addToLog(F("Fan speed HIGH"));
  Parent->getSoundObject()->playOnSound();
}

char *Fan::fanSpeedToText()
{
  if (!*State)
    return (char *)"OFF";
  else if (*HighSpeed)
    return (char *)"HIGH";
  else
    return (char *)"LOW";
}

char *Fan::fanSpeedToNumber()
{
  if (!*State)
    return (char *)"0";
  else if (*HighSpeed)
    return (char *)"2";
  else
    return (char *)"1";
}

// WebServer.setArgBoolean(F("AutoIFan"),GBox -> ModuleSettings -> AutomaticIFan);
// WebServer.setArgString(F("IFanSwitchTemp"), toText(GBox -> ModuleSettings -> IFanSwitchTemp));
// WebServer.setArgBoolean(F("AutoEFan"), GBox -> ModuleSettings -> AutomaticEFan);
// WebServer.setArgString(F("EFanHighHumid"), toText(GBox -> ModuleSettings -> EFanHighHumid));
// WebServer.setArgString(F("EFanLowHumid"), toText(GBox -> ModuleSettings -> EFanLowHumid));
// WebServer.setArgString(F("EFanOffHumid"), toText(GBox -> ModuleSettings -> EFanOffHumid));