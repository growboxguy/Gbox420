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
  logToSerials(F("Aeroponics_NoTank ready"), true, 3);
  if (*SprayEnabled)
  {
    State = AeroNoTankStates::IDLE;
    sprayNow(false); ///< This is a safety feature,start with a spray after a reset
  }
  else
  {
    State = AeroNoTankStates::DISABLED;
  }
}

void Aeroponics_NoTank::report(bool JSONReport)
{
  Common::report(JSONReport); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    strcat_P(LongMessage, (PGM_P)F("\"LS\":\""));
    strcat(LongMessage, toText(LastSprayPressure));
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
    strcat_P(LongMessage, (PGM_P)F("LastSpray:"));
    strcat(LongMessage, toText_pressure(LastSprayPressure));
    strcat_P(LongMessage, (PGM_P)F(" ; Max:"));
    strcat(LongMessage, toText_pressure(*MaxPressure));
    strcat_P(LongMessage, (PGM_P)F(" ; Spray:"));
    strcat(LongMessage, toText_enabledDisabled(*SprayEnabled));
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
  if (State != NewState && *Debug)
  {    
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
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
    appendName(true);
    strcat_P(ShortMessage, (PGM_P)F("duration updated"));
    Parent->addToLog(ShortMessage);
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
    appendName(true);
    strcat_P(ShortMessage, (PGM_P)F("max pressure updated"));
    Parent->addToLog(ShortMessage);
    Parent->getSoundObject()->playOnSound();
  }
}