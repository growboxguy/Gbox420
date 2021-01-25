#include "Aeroponics_Tank.h"

///<TODO: Spray solenoid timeout, SprayTimer review

Aeroponics_Tank::Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::Aeroponics_TankSettings *DefaultSettings, PressurePump *Pump, PressureSensor *FeedbackPressureSensor) : Common(Name)
{ ///< constructor
  this->Parent = Parent;
  this->FeedbackPressureSensor = FeedbackPressureSensor;
  this->Pump = Pump;
  Duration = &DefaultSettings->Duration;           ///< Spray time in seconds
  DayInterval = &DefaultSettings->DayInterval;     ///< Spray every X minutes - With lights ON
  NightInterval = &DefaultSettings->NightInterval; ///< Spray every X minutes - With lights OFF
  MinPressure = &DefaultSettings->MinPressure;     ///< Aeroponics - Turn on pump below this pressure (bar)
  MaxPressure = &DefaultSettings->MaxPressure;     ///< Aeroponics - Turn off pump above this pressure (bar)
  SprayEnabled = &DefaultSettings->SprayEnabled;   ///< Enable/disable misting
  SpraySolenoidClosingDelay = &DefaultSettings->SpraySolenoidClosingDelay;
  logToSerials(F(""), true, 0);  //New line
  logToSerials(F(""), false, 1); //Extra indentation
  SpraySwitch = new Switch(F("SpraySolenoid"), DefaultSettings->SpraySolenoidPin, DefaultSettings->SpraySolenoidNegativeLogic);
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  logToSerials(F("Aeroponics_Tank object created"), true, 2);
  sprayNow(false); ///< This is a safety feature,start with a spray after a reset
}

void Aeroponics_Tank::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("State:"));
  strcat(LongMessage, toText_aeroTankState(State));
  strcat_P(LongMessage, (PGM_P)F(" ; MinPressure:"));
  strcat(LongMessage, toText_pressure(*MinPressure));
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
  logToSerials(&LongMessage, true, 1); ///< Break line, No indentation needed: child class already printed it
}

void Aeroponics_Tank::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  updateState(State);
}

void Aeroponics_Tank::processTimeCriticalStuff()
{
  if (State == AeroTankStates::SPRAY || State == AeroTankStates::STOPSPRAY)
    updateState(State);
}

void Aeroponics_Tank::updateState(AeroTankStates NewState) ///< Without a parameter actualize the current State. When NewState parameter is passed it overwrites State
{
  bool BlockOverWritingState = false;
  if (State != NewState) //When the state changes
  {
    StateTimer = millis(); //Start measuring the time spent in the new state
  }

  switch (NewState)
  {
  case AeroTankStates::DISABLED:
    if (State != NewState)
    {
      SpraySwitch->turnOff();
      Pump->stopPump();
    }
    break;
  case AeroTankStates::IDLE:
    if (State != NewState)
    {
      SpraySwitch->turnOff();
      Pump->stopPump();
    }
    if (Pump->getState() == PressurePumpStates::IDLE || Pump->getState() == PressurePumpStates::DISABLED)
    {
      uint32_t Interval;
      if (DayMode)
        Interval = *DayInterval * 60000; ///< Interval is in minutes
      else
        Interval = *NightInterval * 60000; ///< Interval is in minutes
      if (millis() - SprayTimer >= Interval)
      { ///< if time to start spraying (AeroInterval in Minutes)
        updateState(AeroTankStates::SPRAY);
        BlockOverWritingState = true;
      }
    }
    if (Pump->getState() == PressurePumpStates::IDLE && FeedbackPressureSensor->getPressure() <= *MinPressure)
    { ///< Refill tank: When the pump is idle AND the pressure is below the minimum
      logToSerials(F("Tank recharging"), false, 3);
      updateState(AeroTankStates::REFILL);
      BlockOverWritingState = true;
    }
    break;
  case AeroTankStates::SPRAY:
    if (State != NewState)
    {
      SpraySwitch->turnOn();
      SprayTimer = millis();
    }
    if (millis() - SprayTimer >= *Duration * 1000)
    { ///< if time to stop spraying
      LastSprayPressure = Aeroponics_Tank::FeedbackPressureSensor->getPressure();
      updateState(AeroTankStates::STOPSPRAY);
      BlockOverWritingState = true;
    }
    break;
  case AeroTankStates::STOPSPRAY:
    if (State != NewState)
    {
      SpraySwitch->turnOff();
      SprayTimer = millis();
    }
    if (millis() - SprayTimer >= *SpraySolenoidClosingDelay)
    {                        ///< Closing a solenoid takes time, wait for the spray solenoid to close before opening bypass solenoid to release pressure
      SprayTimer = millis(); ///< Start measuring the time since the last spray
      updateState(AeroTankStates::RELEASE);
      BlockOverWritingState = true;
    }
    break;
  case AeroTankStates::RELEASE:
    if (State != NewState)
    {
      Pump->startBlowOff(); ///< The pressure pump's bypass solenoid is used to release pressure from the misting loop
    }
    if (Pump->getState() != PressurePumpStates::BLOWOFF && Pump->getState() != PressurePumpStates::CLOSINGBYPASS)
    { ///< Wait until pump finishes blowing off and closes the bypass solenoid
      if (*SprayEnabled)
      {
        updateState(AeroTankStates::IDLE);
      }
      else
      {
        updateState(AeroTankStates::DISABLED);
      }
      BlockOverWritingState = true;
    }
    break;
  case AeroTankStates::REFILL:
    if (State != NewState)
    {
      Pump->startPump(true);
    }
    if (FeedbackPressureSensor->readPressure(false) >= *MaxPressure)
    { ///< refill complete, target pressure reached
      logToSerials(F("Tank recharged"), false, 3);
      updateState(AeroTankStates::RELEASE);
      BlockOverWritingState = true;
    }
    if (Pump->getState() == PressurePumpStates::IDLE || Pump->getState() == PressurePumpStates::DISABLED) ///< if pump ended up getting disabled
    {                                                                                                     ///< refill failed, target pressure was not reached before the pump timeout was reached
      logToSerials(F("Recharge failed"), false, 3);
      if (*SprayEnabled)
      {
        updateState(AeroTankStates::IDLE);
      }
      else
      {
        updateState(AeroTankStates::DISABLED);
      }
      BlockOverWritingState = true;
    }
    break;
  case AeroTankStates::DRAIN:
    if (State != NewState)
    {
      Pump->turnBypassOn(true);
      SpraySwitch->turnOn();
    }
    if (FeedbackPressureSensor->readPressure(false) <= 0.1 || Pump->getState() != PressurePumpStates::BYPASSOPEN)
    {
      ///< refill failed, target pressure was not reached before the pump timeout was reached
      if (Pump->getState() == PressurePumpStates::BYPASSOPEN)
        Pump->turnBypassOff();
      SpraySwitch->turnOff();
      logToSerials(F("Tank drained"), false, 3);
      if (*SprayEnabled)
      {
        updateState(AeroTankStates::IDLE);
      }
      else
      {
        updateState(AeroTankStates::DISABLED);
      }
      BlockOverWritingState = true;
    }
    break;
  case AeroTankStates::MIX:
    if (State != NewState)
    {
      Pump->startMixing();
    }
    if (Pump->getState() != PressurePumpStates::MIX)
    { /// When mixing finished (Mixing runs till pump timeout)
      if (*SprayEnabled)
      {
        updateState(AeroTankStates::IDLE);
      }
      else
      {
        updateState(AeroTankStates::DISABLED);
      }
      BlockOverWritingState = true;
    }
    break;
  }

  if (State != NewState && !BlockOverWritingState)
  {
    State = NewState;
  }
}

void Aeroponics_Tank::sprayNow(bool UserRequest)
{
  if (Pump->getState() == PressurePumpStates::IDLE || Pump->getState() == PressurePumpStates::DISABLED) //Only allow spraying when pump and bypass solenoid is off
  {
    updateState(AeroTankStates::SPRAY);
    if (UserRequest)
    {
      Parent->getSoundObject()->playOnSound();
      Parent->addToLog(F("Spraying"));
    }
    else
    {
      logToSerials(F("Spraying"), true, 3);
    }
  }
  else
  {
    Parent->getSoundObject()->playOffSound();
    logToSerials(F("Cannot spray now"), true, 3);
  }
}

void Aeroponics_Tank::sprayOff(bool UserRequest)
{
  if (State == AeroTankStates::SPRAY)
  {
    updateState(AeroTankStates::STOPSPRAY);
  }
  if (UserRequest)
  {
    Parent->getSoundObject()->playOffSound();
    Parent->addToLog(F("Spray OFF"));
  }
  else
  {
    logToSerials(F("Spray OFF"), true, 3);
  }
}

void Aeroponics_Tank::refillTank()
{
  if (State == AeroTankStates::IDLE || State == AeroTankStates::DISABLED)
  {
    Parent->addToLog(F("REFILL tank"));
    updateState(AeroTankStates::REFILL);
  }
}

void Aeroponics_Tank::drainTank()
{
  if (State == AeroTankStates::IDLE || State == AeroTankStates::DISABLED)
  {
    Parent->addToLog(F("Draining tank"));
    updateState(AeroTankStates::DRAIN);
  }
}

void Aeroponics_Tank::startMixing()
{
  if (State == AeroTankStates::IDLE || State == AeroTankStates::DISABLED)
  {
    Parent->addToLog(F("Mixing"));
    updateState(AeroTankStates::MIX);
  }
}

void Aeroponics_Tank::setDayMode(bool State)
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

void Aeroponics_Tank::setDayInterval(uint16_t Interval)
{
  if (*DayInterval != Interval && Interval > 0)
  {
    *DayInterval = Interval;
    Parent->getSoundObject()->playOnSound();
  }
}

void Aeroponics_Tank::setNightInterval(uint16_t Interval)
{
  if (*NightInterval != Interval && Interval > 0)
  {
    *NightInterval = Interval;
    Parent->getSoundObject()->playOnSound();
  }
}

void Aeroponics_Tank::setDuration(float duration)
{
  if (*Duration != duration && duration > 0)
  {
    *Duration = duration;
    Parent->addToLog(F("Duration updated"));
    Parent->getSoundObject()->playOnSound();
  }
}

void Aeroponics_Tank::setSprayOnOff(bool State)
{
  *SprayEnabled = State;
  if (*SprayEnabled)
  {
    updateState(AeroTankStates::IDLE);
    Parent->addToLog(F("Spray enabled"));
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    updateState(AeroTankStates::DISABLED);
    Parent->addToLog(F("Spray disabled"));
    Parent->getSoundObject()->playOffSound();
  }
}

char *Aeroponics_Tank::sprayStateToText()
{
  return toText_onOff(*SprayEnabled);
}

void Aeroponics_Tank::setMinPressure(float Pressure)
{
  if (*MinPressure != Pressure && Pressure > 0)
  {
    *MinPressure = Pressure;
  }
}

void Aeroponics_Tank::setMaxPressure(float Pressure)
{
  if (*MaxPressure != Pressure && Pressure > 0)
  {
    *MaxPressure = Pressure;
    Parent->addToLog(F("Pressure limits updated"));
    Parent->getSoundObject()->playOnSound();
  }
}

bool Aeroponics_Tank::getSprayEnabled()
{
  return *SprayEnabled;
}

AeroTankStates Aeroponics_Tank::getState()
{
  return State;
}

float Aeroponics_Tank::getLastSprayPressure()
{
  return LastSprayPressure;
}