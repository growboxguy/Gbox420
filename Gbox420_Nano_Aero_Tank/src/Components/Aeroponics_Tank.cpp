#include "Aeroponics_Tank.h"

///<TODO: Spray solenoid timeout, SprayTimer review

Aeroponics_Tank::Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressurePump *Pump, PressureSensor *FeedbackPressureSensor) : Common(Name)
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
  logToSerials(F("Aeroponics_Tank ready"), true, 3);
  if (*SprayEnabled)
  {
    State = AeroTankStates::IDLE;
    sprayNow(false); ///< This is a safety feature,start with a spray after a reset
  }
  else
  {
    State = AeroTankStates::DISABLED;
  }
}

void Aeroponics_Tank::report(bool JSONReport)
{
  Common::report(JSONReport); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {     
    strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
    strcat(LongMessage, toText((int)State));
    strcat_P(LongMessage, (PGM_P)F("\",\"Mn\":\""));
    strcat(LongMessage, toText(*MinPressure));
    strcat_P(LongMessage, (PGM_P)F("\",\"Mx\":\""));
    strcat(LongMessage, toText(*MaxPressure));
    strcat_P(LongMessage, (PGM_P)F("\",\"SE\":\""));
    strcat(LongMessage, toText(*SprayEnabled));
    strcat_P(LongMessage, (PGM_P)F("\",\"DM\":\""));
    strcat(LongMessage, toText(DayMode));
    strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
    strcat(LongMessage, toText(*Duration));
    strcat_P(LongMessage, (PGM_P)F("\",\"DI\":\""));
    strcat(LongMessage, toText(*DayInterval));
    strcat_P(LongMessage, (PGM_P)F("\",\"NI\":\""));
    strcat(LongMessage, toText(*NightInterval));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else //Print a report to the Serial console
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
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
}

void Aeroponics_Tank::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  updateState(State);
}

void Aeroponics_Tank::processTimeCriticalStuff() ///< Called every 0.1sec
{
  if (State == AeroTankStates::SPRAY || State == AeroTankStates::STOPSPRAY)
    updateState(State);
}

void Aeroponics_Tank::updateState(AeroTankStates NewState) ///< Without a parameter actualize the current State. When NewState parameter is passed it overwrites State
{
  bool BlockOverWritingState = false; //Used when a state transitions to a new state
  if (State != NewState && *Debug)
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)Name);
    strcat_P(LongMessage, (PGM_P)F(" state: "));
    strcat(LongMessage, toText_aeroTankState(State));
    strcat_P(LongMessage, (PGM_P)F(" -> "));
    strcat(LongMessage, toText_aeroTankState(NewState));
    logToSerials(&LongMessage, true, 3);
  }

  switch (NewState)
  {
  case AeroTankStates::DISABLED: ///< Turns off spray cycle and tank pressure monitoring, plants die within hours in this state!!
    if (State != NewState)
    {
      *SprayEnabled = false;
      SpraySwitch->turnOff();
      Pump->stopPump();
    }
    break;
  case AeroTankStates::IDLE: ///< Spray cycle and tank pressure monitoring active
    if (State != NewState)
    {
      *SprayEnabled = true;
      SpraySwitch->turnOff();
      Pump->stopPump();
    }
    if (Pump->getState() == PressurePumpStates::IDLE || Pump->getState() == PressurePumpStates::DISABLED)
    { ///Start spraying only when the pump is not busy
      uint32_t Interval;
      if (DayMode)
        Interval = *DayInterval * 60000; ///< Interval is in minutes
      else
        Interval = *NightInterval * 60000; ///< Interval is in minutes
      if (millis() - SprayTimer >= Interval)
      { ///< Time to start spraying
        updateState(AeroTankStates::SPRAY);
        BlockOverWritingState = true;
      }
    }
    if (Pump->getState() == PressurePumpStates::IDLE && FeedbackPressureSensor->getPressure() <= *MinPressure)
    { ///< Refill tank: When the pump is idle AND the pressure is below the minimum
      updateState(AeroTankStates::REFILL);
      BlockOverWritingState = true;
    }
    break;
  case AeroTankStates::SPRAY: ///< Turns on spray solenoid
    if (State != NewState)
    {
      *SprayEnabled = true;
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
  case AeroTankStates::STOPSPRAY: //< Closes the spray solenoid (Takes around ~0.5sec, check solenoid specification and adjust SpraySolenoidClosingDelay in Settings.h)
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
  case AeroTankStates::RELEASE: //< Release remaining pressure from the misting loop AND pump output side
    if (State != NewState)
    {
      Pump->startBlowOff();
    }
    if (Pump->getState() != PressurePumpStates::BLOWOFF && Pump->getState() != PressurePumpStates::BYPASSCLOSE)
    { ///< Wait until pump finishes blowing off and closes the bypass solenoid
      updateState(AeroTankStates::IDLE);
      BlockOverWritingState = true;
    }
    break;
  case AeroTankStates::REFILL: ///< Refills the pressure tank. Re-enables the Spray cycle once finished.
    if (State != NewState)
    {
      Pump->startPump(true);
    }
    if (FeedbackPressureSensor->readPressure(false) >= *MaxPressure)
    { ///< refill complete, target pressure reached
      updateState(AeroTankStates::RELEASE);
      BlockOverWritingState = true;
    }
    if (Pump->getState() == PressurePumpStates::IDLE || Pump->getState() == PressurePumpStates::DISABLED)
    { ///< refill failed, target pressure was not reached before the pump timeout
      //logToSerials(F("Recharge failed"), true, 3);
      updateState(AeroTankStates::IDLE);
      BlockOverWritingState = true;
    }
    break;
  case AeroTankStates::DRAIN: ///< Drains the pressure tank. Will NOT re-enable the Spray cycle if it was disabled before draining.
    if (State != NewState)
    {
      Pump->turnBypassOn(true);
      SpraySwitch->turnOn();
    }
    if (FeedbackPressureSensor->readPressure(false) <= 0.1 || (Pump->getState() == PressurePumpStates::IDLE || Pump->getState() == PressurePumpStates::DISABLED))
    { ///< Pressure close to zero OR the pump's bypass solenoid reached it's maximum open time and closed itself
      if (Pump->getState() == PressurePumpStates::BYPASSOPEN)
        Pump->turnBypassOff();
      SpraySwitch->turnOff();
      if (*SprayEnabled)
      {
        updateState(AeroTankStates::IDLE); /// If spray cycle was enabled before draining: Return to IDLE (will trigger a refill)
      }
      else
      {
        updateState(AeroTankStates::DISABLED); /// If spray cycle was disabled before draining: Return to DISABLED (plants will die)
      }
      BlockOverWritingState = true;
    }
    break;
  case AeroTankStates::MIX: ///< Opens bypass valve and turns on the pump to mix the reservoir. Re-enables the Spray cycle once finished.
    if (State != NewState)
    {
      Pump->startMixing();
    }
    if (Pump->getState() != PressurePumpStates::MIX)
    { /// When mixing finished (Mixing runs till pump timeout)
      updateState(AeroTankStates::IDLE);
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
  if (UserRequest)
  {
    Parent->getSoundObject()->playOnSound();
  }
  if (State == AeroTankStates::IDLE || State == AeroTankStates::DISABLED)
  {
    updateState(AeroTankStates::SPRAY);
  }
}

void Aeroponics_Tank::sprayOff(bool UserRequest)
{
  if (UserRequest)
  {
    Parent->getSoundObject()->playOffSound();
  }
  if (State == AeroTankStates::SPRAY)
  {
    updateState(AeroTankStates::STOPSPRAY);
  }
  else
  {
    updateState(AeroTankStates::IDLE); /// If spray cycle was enabled before: Return to IDLE
  }
}

void Aeroponics_Tank::refillTank()
{
  if (State == AeroTankStates::IDLE || State == AeroTankStates::DISABLED)
  {
    Parent->getSoundObject()->playOnSound();
    updateState(AeroTankStates::REFILL);
  }
}

void Aeroponics_Tank::drainTank()
{
  if (State == AeroTankStates::IDLE || State == AeroTankStates::DISABLED)
  {
    Parent->getSoundObject()->playOnSound();
    updateState(AeroTankStates::DRAIN);
  }
}

void Aeroponics_Tank::startMixing()
{
  if (State == AeroTankStates::IDLE || State == AeroTankStates::DISABLED)
  {
    Parent->getSoundObject()->playOnSound();
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
    Parent->getSoundObject()->playOnSound();
  }
}

void Aeroponics_Tank::setSprayOnOff(bool State)
{
  if (State)
  {
    updateState(AeroTankStates::IDLE);
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    updateState(AeroTankStates::DISABLED);
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