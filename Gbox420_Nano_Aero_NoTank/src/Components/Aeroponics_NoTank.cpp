#include "Aeroponics_NoTank.h"

Aeroponics_NoTank::Aeroponics_NoTank(const __FlashStringHelper *Name, 
                                     Module *Parent, 
                                     Settings::AeroponicsSettings *DefaultSettings, 
                                     PressureSensor *FeedbackPressureSensor, 
                                     PressurePump *Pump) 
  : Common(Name), 
    Parent(Parent), 
    FeedbackPressureSensor(FeedbackPressureSensor), 
    Pump(Pump), 
    SprayEnabled(DefaultSettings->SprayEnabled),   ///< Pointer to SprayEnabled
    Duration(DefaultSettings->Duration),           ///< Pointer to Duration
    DayInterval(DefaultSettings->DayInterval),     ///< Pointer to DayInterval
    NightInterval(DefaultSettings->NightInterval), ///< Pointer to NightInterval
    MaxPressure(DefaultSettings->MaxPressure),     ///< Pointer to MaxPressure
    State(AeroNoTankStates::IDLE),   ///< Initialize to IDLE state
    RunTillTimeout(false),            ///< Default RunTillTimeout to false
    LastSprayPressure(0),             ///< Initialize pressure to 0
    DayMode(true)                     ///< Default to Day Mode
{
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  logToSerials(F("Aeroponics_NoTank ready"), true, 3);
  if (SprayEnabled)
  {
    sprayNow(false); ///< Start with a spray after a reset if enabled
  }
  else
  {
    State = AeroNoTankStates::DISABLED;  ///< If not enabled, set state to DISABLED
  }
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void Aeroponics_NoTank::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, getStateText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"LS\":\""));
  strcat(LongMessage, getLastSprayPressureText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"MN\":\""));
  strcat(LongMessage, getMinPressureText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"MX\":\""));
  strcat(LongMessage, getMaxPressureText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"SE\":\""));
  strcat(LongMessage, getSprayEnabledText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"DM\":\""));
  strcat(LongMessage, getDayModeText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, getDurationText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"DI\":\""));
  strcat(LongMessage, getDayIntervalText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"NI\":\""));
  strcat(LongMessage, getNightIntervalText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void Aeroponics_NoTank::refresh_Sec()
{
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
  bool BlockOverWritingState = false; // Used when a state transitions to a new state
  if (State != NewState && Debug)
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
      Interval = DayInterval * 60000; ///< Duration is miliseconds, DayInterval is Minutes
    }
    else
    {
      Interval = NightInterval * 60000; ///< Duration is miliseconds
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
    if (FeedbackPressureSensor->readPressure(false) > MaxPressure || millis() - SprayTimer >= Duration * 1000)
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

AeroNoTankStates Aeroponics_NoTank::getState()
{
  return State;
}

char *Aeroponics_NoTank::getStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_aeroNoTankState(State);
  }
  else
  {
    return toText((int)State);
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
    Parent->getSoundObject()->playOnOffSound(DayMode);
  }
}

char *Aeroponics_NoTank::getDayModeText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_onOff(DayMode);
  }
  else
  {
    return toText(DayMode);
  }
}

void Aeroponics_NoTank::setDayInterval(int Interval)
{
  if (DayInterval != Interval && Interval > 0)
  {
    DayInterval = Interval;
    Parent->getSoundObject()->playOnSound();
  }
}

int Aeroponics_NoTank::getDayInterval()
{
  return DayInterval;
}

char *Aeroponics_NoTank::getDayIntervalText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_minute(DayInterval);
  }
  else
  {
    return toText(DayInterval);
  }
}

void Aeroponics_NoTank::setNightInterval(int Interval)
{
  if (NightInterval != Interval && Interval > 0)
  {
    NightInterval = Interval;
    Parent->getSoundObject()->playOnSound();
  }
}

int Aeroponics_NoTank::getNightInterval()
{
  return NightInterval;
}

char *Aeroponics_NoTank::getNightIntervalText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_minute(NightInterval);
  }
  else
  {
    return toText(NightInterval);
  }
}

void Aeroponics_NoTank::setDuration(float duration)
{
  if (Duration != duration && duration > 0)
  {
    Duration = duration;
    Parent->addToLog(getName(F("duration updated")));
    Parent->getSoundObject()->playOnSound();
  }
}

void Aeroponics_NoTank::setSprayOnOff(bool State)
{
  SprayEnabled = State;
  if (SprayEnabled)
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
  return SprayEnabled;
}

char *Aeroponics_NoTank::getSprayEnabledText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_enabledDisabled(SprayEnabled);
  }
  else
  {
    return toText(SprayEnabled);
  }
}

float Aeroponics_NoTank::getDuration()
{
  return Duration;
}

char *Aeroponics_NoTank::getDurationText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_second(Duration);
  }
  else
  {
    return toText(Duration);
  }
}

char *Aeroponics_NoTank::sprayStateToText()
{
  return toText_onOff(SprayEnabled);
}

float Aeroponics_NoTank::getLastSprayPressure()
{
  return LastSprayPressure;
}

char *Aeroponics_NoTank::getLastSprayPressureText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_pressure(LastSprayPressure);
  }
  else
  {
    return toText(LastSprayPressure);
  }
}

float Aeroponics_NoTank::getPressure()
{
  return FeedbackPressureSensor->getPressure();
}

void Aeroponics_NoTank::setMinPressure(float Pressure)
{
  if (MinPressure != Pressure && Pressure > 0)
  {
    MinPressure = Pressure;
  }
}

char *Aeroponics_NoTank::getMinPressureText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_pressure(MinPressure);
  }
  else
  {
    return toText(MinPressure);
  }
}

void Aeroponics_NoTank::setMaxPressure(float Pressure)
{
  if (MaxPressure != Pressure && Pressure > 0)
  {
    MaxPressure = Pressure;
    Parent->addToLog(getName(F("max pressure updated")));
    Parent->getSoundObject()->playOnSound();
  }
}

char *Aeroponics_NoTank::getMaxPressureText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_pressure(MaxPressure);
  }
  else
  {
    return toText(MaxPressure);
  }
}