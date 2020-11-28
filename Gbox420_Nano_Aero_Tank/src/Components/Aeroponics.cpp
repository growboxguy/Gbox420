#include "Aeroponics.h"

///< This is a virtual class (cannot be instanciated), parent of two classes:
///< -Aeroponics_NoTank: High pressure pump is directly connected to the aeroponics tote
///< -Aeroponics_Tank: A pressure tank is added between the high pressure pump and aeroponics tote, requires an extra solenoid for spraying

Aeroponics::Aeroponics(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) : Common(Name)
{ ///< constructor
  this->Parent = Parent;
  SprayEnabled = &DefaultSettings->SprayEnabled;   ///< Enable/disable misting
  DayInterval = &DefaultSettings->DayInterval;     ///< Spray every X minutes - With lights ON
  DayDuration = &DefaultSettings->DayDuration;     ///< Spray time in seconds - With lights ON
  NightInterval = &DefaultSettings->NightInterval; ///< Spray every X minutes - With lights OFF
  NightDuration = &DefaultSettings->NightDuration; ///< Spray time in seconds - With lights OFF
  MaxPressure = &DefaultSettings->MaxPressure;     ///< Aeroponics - Turn off pump above this pressure (bar)
  this->FeedbackPressureSensor = FeedbackPressureSensor;
  this->Pump = Pump;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
}

void Aeroponics::report()
{                                                  ///< report status to Serial output, runs after the child class`s report function
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F(" ; MaxPressure:"));
  strcat(LongMessage, toText_pressure(*MaxPressure));
  strcat_P(LongMessage, (PGM_P)F(" ; SprayEnabled:"));
  strcat(LongMessage, toText_yesNo(SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F(" ; DayMode:"));
  strcat(LongMessage, toText_enabledDisabled(DayMode));
  strcat_P(LongMessage, (PGM_P)F(" ; DayInterval:"));
  strcat(LongMessage, toText_minute(*DayInterval));
  strcat_P(LongMessage, (PGM_P)F(" ; DayDuration:"));
  strcat(LongMessage, toText_second(*DayDuration));
  strcat_P(LongMessage, (PGM_P)F(" ; NightInterval:"));
  strcat(LongMessage, toText_minute(*NightInterval));
  strcat_P(LongMessage, (PGM_P)F(" ; NightDuration:"));
  strcat(LongMessage, toText_second(*NightDuration));
  logToSerials(&LongMessage, true, 0); ///< Break line, No indentation needed: child class already printed it
}

void Aeroponics::setDayMode(bool State)
{
  if (DayMode != State)
  {
    DayMode = State;
    if (DayMode)
    {
      Parent->getSoundObject()->playOnSound();
    }
    else
    {
      Parent->getSoundObject()->playOffSound();
    }
  }
}

void Aeroponics::setDayInterval(int Interval)
{
  if (*DayInterval != Interval && Interval > 0)
  {
    *DayInterval = Interval;
    Parent->getSoundObject()->playOnSound();
  }
}

int Aeroponics::getDayInterval()
{
  return *DayInterval;
}

char *Aeroponics::getDayIntervalText()
{
  return toText(*DayInterval);
}

void Aeroponics::setNightInterval(int Interval)
{
  if (*NightInterval != Interval && Interval > 0)
  {
    *NightInterval = Interval;
    Parent->getSoundObject()->playOnSound();
  }
}

int Aeroponics::getNightInterval()
{
  return *NightInterval;
}

char *Aeroponics::getNightIntervalText()
{
  return toText(*NightInterval);
}

void Aeroponics::setDayDuration(int Duration)
{
  if (*DayDuration != Duration && Duration > 0)
  {
    *DayDuration = Duration;
    Parent->addToLog(F("Day duration updated"));
    Parent->getSoundObject()->playOnSound();
  }
}

void Aeroponics::setNightDuration(int Duration)
{
  if (*NightDuration != Duration && Duration > 0)
  {
    *NightDuration = Duration;
    Parent->addToLog(F("Night duration updated"));
    Parent->getSoundObject()->playOnSound();
  }
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

bool Aeroponics::getSprayEnabled()
{
  return *SprayEnabled;
}

int Aeroponics::getDayDuration()
{
  return *DayDuration;
}

int Aeroponics::getNightDuration()
{
  return *NightDuration;
}

char *Aeroponics::getSprayDurationText()
{
  return toText(*DayDuration);
}

char *Aeroponics::sprayStateToText()
{
  return toText_onOff(*SprayEnabled);
}

float Aeroponics::getLastSprayPressure()
{
  return LastSprayPressure;
}

char *Aeroponics::getLastSprayPressureText(bool IncludeCurrentPressure)
{
  memset(&ShortMessage[0], 0, sizeof(ShortMessage)); ///< clear variable

  toText_pressure(LastSprayPressure); ///< loads the Last pressure measured during spraying
  if (IncludeCurrentPressure)
  {
    strcat_P(ShortMessage, (PGM_P)F(" ["));
    dtostrf(FeedbackPressureSensor->getPressure(), 4, 2, ShortMessage);
    strcat_P(ShortMessage, (PGM_P)F("]"));
  }
  return ShortMessage;
}


///< Pump controls

float Aeroponics::getPressure()
{
  return FeedbackPressureSensor->getPressure();
}

void Aeroponics::setMinPressure(float Pressure)
{
  if (*MinPressure != Pressure && Pressure > 0)
  {
    *MinPressure = Pressure;
  }
}

void Aeroponics::setMaxPressure(float Pressure)
{
  if (*MaxPressure != Pressure && Pressure > 0)
  {
    *MaxPressure = Pressure;
    Parent->addToLog(F("Tank limits updated"));
    Parent->getSoundObject()->playOnSound();
  }
}