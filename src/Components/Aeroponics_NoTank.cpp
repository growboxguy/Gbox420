#include "Aeroponics_NoTank.h"

Aeroponics_NoTank::Aeroponics_NoTank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, PressurePump *Pump) : Common(Name)
{
  this->Parent = Parent;
  this->FeedbackPressureSensor = FeedbackPressureSensor;
  this->Pump = Pump;
  SprayEnabled = &DefaultSettings->SprayEnabled;   ///< Enable/disable misting
  Duration = &DefaultSettings->Duration;           ///< Spray time in seconds
  DayInterval = &DefaultSettings->DayInterval;     ///< Spray every X minutes - With lights ON
  NightInterval = &DefaultSettings->NightInterval; ///< Spray every X minutes - With lights OFF
  MaxPressure = &DefaultSettings->MaxPressure;     ///< Aeroponics - Turn off pump above this pressure (bar)
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  logToSerials(F("Aeroponics_NoTank object created"), true, 1);
  sprayNow(false); ///< This is a safety feature,start with a spray after a reset
}

void Aeroponics_NoTank::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("LastSprayPressure:"));
  strcat(LongMessage, toText_pressure(LastSprayPressure));
  strcat_P(LongMessage, (PGM_P)F(" ; MaxPressure:"));
  strcat(LongMessage, toText_pressure(*MaxPressure));
  strcat_P(LongMessage, (PGM_P)F(" ; SprayEnabled:"));
  strcat(LongMessage, toText_yesNo(SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F(" ; DayMode:"));
  strcat(LongMessage, toText_enabledDisabled(DayMode));
  strcat_P(LongMessage, (PGM_P)F(" ; Duration:"));
  strcat(LongMessage, toText_second(*Duration));
  strcat_P(LongMessage, (PGM_P)F(" ; DayInterval:"));
  strcat(LongMessage, toText_minute(*DayInterval));
  strcat_P(LongMessage, (PGM_P)F(" ; NightInterval:"));
  strcat(LongMessage, toText_minute(*NightInterval));
  logToSerials(&LongMessage, true, 1);
}

void Aeroponics_NoTank::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  updateState(State);
}

void Aeroponics_NoTank::processTimeCriticalStuff() ///< Runs every 0.1 sec
{
  if (State == AeroNoTankStates::PRIMING || State == AeroNoTankStates::SPRAY)
    updateState(State);
}

void Aeroponics_NoTank::updateState(AeroNoTankStates NewState) ///< Without a parameter actualize the current State. When NewState parameter is passed it overwrites State
{
  bool BlockOverWritingState = false; //Used when a state transitions to a new state
  if (State != NewState)
  {
    memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
    strcat_P(LongMessage, (PGM_P)Name);
    strcat_P(LongMessage, (PGM_P)F(" state: "));
    strcat(LongMessage, toText_aeroNoTankState(State));
    strcat_P(LongMessage, (PGM_P)F(" -> "));
    strcat(LongMessage, toText_aeroNoTankState(NewState));
    logToSerials(&LongMessage, true, 3);
  }

  switch (NewState)
  {
  case AeroNoTankStates::DISABLED: ///< Turns off spray cycle and tank pressure monitoring, plants die within hours in this state!!
    if (State != NewState)
    {
      Pump->stopPump();
    }
    break;
  case AeroNoTankStates::IDLE: ///< Spray cycle and tank pressure monitoring active
    if (State != NewState)
    {
      Pump->stopPump();
    }
    uint32_t Interval;
    if (DayMode)
    {
      Interval = *DayInterval * 60000; ///< Duration is miliseconds, DayInterval is Minutes
    }
    else
    {
      Interval = *NightInterval * 60000; ///< Duration is miliseconds
    }
    if (millis() - SprayTimer >= Interval)
    { ///< if time to start spraying
      updateState(AeroNoTankStates::PRIMING);
      BlockOverWritingState = true;
    }
    break;
  case AeroNoTankStates::PRIMING: ///< Turns off spray cycle and tank pressure monitoring, plants die within hours in this state!!
    if (State != NewState)
    {
      Pump->startPump(true);
    }
    if (Pump->getState() != PressurePumpStates::PRIMING)
    {
      updateState(AeroNoTankStates::SPRAY);
      BlockOverWritingState = true;
    }
    break;
  case AeroNoTankStates::SPRAY: ///< Turns on spray solenoid
    if (State != NewState)
    {
      SprayTimer = millis();
    }
    if (FeedbackPressureSensor->readPressure(false) > *MaxPressure || millis() - SprayTimer >= *Duration * 1000)
    {
      LastSprayPressure = FeedbackPressureSensor->getPressure();
      updateState(AeroNoTankStates::RELEASE);
      BlockOverWritingState = true;
    }
    break;
  case AeroNoTankStates::RELEASE: //< Closes the spray solenoid (Takes around ~0.5sec, check solenoid specification and adjust SpraySolenoidClosingDelay in Settings.h)
    if (State != NewState)
    {
      Pump->stopPump();
    }
    if (Pump->getState() == PressurePumpStates::IDLE)
    {
      updateState(AeroNoTankStates::IDLE);
      BlockOverWritingState = true;
    }
    if (Pump->getState() == PressurePumpStates::DISABLED)
    {
      updateState(AeroNoTankStates::DISABLED);
      BlockOverWritingState = true;
    }
    break;
  case AeroNoTankStates::MIX: //< Release remaining pressure from the misting loop AND pump output side
    if (State != NewState)
    {
      Pump->startMixing();
    }
    if (Pump->getState() != PressurePumpStates::MIX)
    { /// When mixing finished (Mixing runs till pump timeout)
      updateState(AeroNoTankStates::IDLE);
      BlockOverWritingState = true;
    }
    break;
  }

  if (State != NewState && !BlockOverWritingState)
  {
    State = NewState;
  }
}

void Aeroponics_NoTank::sprayNow(bool UserRequest)
{
  if (UserRequest)
  {
    Parent->getSoundObject()->playOnSound();
  }
  updateState(AeroNoTankStates::PRIMING);
}

void Aeroponics_NoTank::sprayOff()
{
  if (State == AeroNoTankStates::SPRAY)
    updateState(AeroNoTankStates::RELEASE);
}

void Aeroponics_NoTank::setDayMode(bool State)
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

void Aeroponics_NoTank::setDayInterval(int Interval)
{
  if (*DayInterval != Interval && Interval > 0)
  {
    *DayInterval = Interval;
    Parent->getSoundObject()->playOnSound();
  }
}

int Aeroponics_NoTank::getDayInterval()
{
  return *DayInterval;
}

char *Aeroponics_NoTank::getDayIntervalText()
{
  return toText(*DayInterval);
}

void Aeroponics_NoTank::setNightInterval(int Interval)
{
  if (*NightInterval != Interval && Interval > 0)
  {
    *NightInterval = Interval;
    Parent->getSoundObject()->playOnSound();
  }
}

int Aeroponics_NoTank::getNightInterval()
{
  return *NightInterval;
}

char *Aeroponics_NoTank::getNightIntervalText()
{
  return toText(*NightInterval);
}

void Aeroponics_NoTank::setDuration(float duration)
{
  if (*Duration != duration && duration > 0)
  {
    *Duration = duration;
    Parent->addToLog(F("Duration updated"));
    Parent->getSoundObject()->playOnSound();
  }
}

void Aeroponics_NoTank::setSprayOnOff(bool State)
{
  *SprayEnabled = State;
  if (*SprayEnabled)
  {
    updateState(AeroNoTankStates::IDLE);
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    updateState(AeroNoTankStates::DISABLED);
    Parent->getSoundObject()->playOffSound();
  }
}

bool Aeroponics_NoTank::getSprayEnabled()
{
  return *SprayEnabled;
}

float Aeroponics_NoTank::getDuration()
{
  return *Duration;
}

char *Aeroponics_NoTank::getSprayDurationText()
{
  return toText(*Duration);
}

char *Aeroponics_NoTank::sprayStateToText()
{
  return toText_onOff(*SprayEnabled);
}

float Aeroponics_NoTank::getLastSprayPressure()
{
  return LastSprayPressure;
}

float Aeroponics_NoTank::getPressure()
{
  return FeedbackPressureSensor->getPressure();
}

void Aeroponics_NoTank::setMinPressure(float Pressure)
{
  if (*MinPressure != Pressure && Pressure > 0)
  {
    *MinPressure = Pressure;
  }
}

void Aeroponics_NoTank::setMaxPressure(float Pressure)
{
  if (*MaxPressure != Pressure && Pressure > 0)
  {
    *MaxPressure = Pressure;
    Parent->addToLog(F("Max pressure updated"));
    Parent->getSoundObject()->playOnSound();
  }
}