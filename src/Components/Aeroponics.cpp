#include "Aeroponics.h"

/// This is a virtual class (cannot be instanciated), parent of two classes:
/// -Aeroponics_NoTank: High pressure pump is directly connected to the aeroponics tote
/// -Aeroponics_Tank: A pressure tank is added between the high pressure pump and aeroponics tote, requires an extra solenoid for spraying

Aeroponics::Aeroponics(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor) : Common(Name)
{ ///constructor
  this->Parent = Parent;
  BypassSolenoidPin = &DefaultSettings->BypassSolenoidPin;
  PumpPin = &DefaultSettings->PumpPin;
  SprayEnabled = &DefaultSettings->SprayEnabled; ///Enable/disable misting
  Interval = &DefaultSettings->Interval;         ///Aeroponics - Spray every 15 minutes
  Duration = &DefaultSettings->Duration;         ///Aeroponics - Spray time in seconds
  PumpTimeout = &DefaultSettings->PumpTimeout;   /// Aeroponics - Max pump run time in minutes, measue zero to max pressuretank refill time and adjust accordingly
  PrimingTime = &DefaultSettings->PrimingTime;   /// Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.
  this->FeedbackPressureSensor = FeedbackPressureSensor;
  pinMode(*BypassSolenoidPin, OUTPUT);
  digitalWrite(*BypassSolenoidPin, HIGH); ///initialize in off state
  pinMode(*PumpPin, OUTPUT);
  digitalWrite(*PumpPin, HIGH);          ///initialize in off state
  Parent->addToReportQueue(this);          ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Sec(this);     ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method  
}

void Aeroponics::report()
{                                                  ///report status to Serial output, runs after the child class`s report function
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F(" ; SprayEnabled:"));
  strcat(LongMessage, yesNoToText(SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F(" ; Interval:"));
  strcat(LongMessage, toText(*Interval));
  strcat_P(LongMessage, (PGM_P)F(" ; Duration:"));
  strcat(LongMessage, toText(*Duration));
  strcat_P(LongMessage, (PGM_P)F(" ; PumpTimeout:"));
  strcat(LongMessage, toText(*PumpTimeout));
  strcat_P(LongMessage, (PGM_P)F(" ; PrimingTime:"));
  strcat(LongMessage, toText(*PrimingTime));
  logToSerials(&LongMessage, true, 0); ///Break line, No indentation needed: child class already printed it
}

void Aeroponics::checkRelays()
{
  if (PumpOn)
    digitalWrite(*PumpPin, LOW);
  else
    digitalWrite(*PumpPin, HIGH);
  if (BypassSolenoidOn)
    digitalWrite(*BypassSolenoidPin, LOW);
  else
    digitalWrite(*BypassSolenoidPin, HIGH);
}

void Aeroponics::setPumpOn(bool UserRequest)
{ ///turns pump on, UserRequest is true if it was triggered from the website
  if (UserRequest)
  { ///if the pump was turned on from the web interface, not by the automation
    Parent->addToLog(F("Pump ON"));
    Parent->getSoundObject()->playOnSound();
    PumpOK = true; ///re-enable pump
  }
  MixInProgress = UserRequest; ///If pump was turned on from the web interface first run an air bleeding cycle
  PumpOn = true;
  PumpTimer = millis();
  checkRelays();
}

void Aeroponics::setPumpOff(bool UserRequest)
{
  if (UserRequest)
  { ///if the pump was turned off from the web interface, not by the automation
    Parent->addToLog(F("Pump OFF"));
    Parent->getSoundObject()->playOffSound();
    PumpOK = true; ///re-enable pump
  }
  MixInProgress = false;
  PumpOn = false;
  ///if(!BlowOffInProgress)BypassSolenoidOn = false; ///Close bypass valve
  PumpTimer = millis();
  checkRelays();
}

void Aeroponics::PumpDisable()
{
  setPumpOff(false);
  PumpOK = false;
  Parent->addToLog(F("Pump disabled"));
}

void Aeroponics::Mix()
{
  PumpOn = true;
  MixInProgress = true;
  PumpOK = true;
  BypassSolenoidOn = true;
  PumpTimer = millis();
  checkRelays();
  Parent->getSoundObject()->playOnSound();
  Parent->addToLog(F("Mixing nutrients"));
}

void Aeroponics::setInterval(int interval)
{
  *Interval = interval;
  SprayTimer = millis();
}

char *Aeroponics::getInterval()
{
  return toText(*Interval);
}

void Aeroponics::setDuration(int duration)
{
  *Duration = duration;
  SprayTimer = millis();
  Parent->addToLog(F("Spray time updated"));
}

char *Aeroponics::getDuration()
{
  return toText(*Duration);
}

void Aeroponics::setPumpTimeout(int Timeout)
{
  *PumpTimeout = Timeout;
  Parent->addToLog(F("Aero pump timeout updated"));
}

void Aeroponics::setPrimingTime(int Timing)
{
  *PrimingTime = Timing;
  Parent->addToLog(F("Aero priming time updated"));
}

void Aeroponics::setSprayOnOff(bool State)
{
  *SprayEnabled = State;
  if (*SprayEnabled)
  {
    Parent->addToLog(F("Aero spray enabled"));
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    Parent->addToLog(F("Aero spray disabled"));
    Parent->getSoundObject()->playOffSound();
  }
}