#include "Aeroponics_Tank.h"

Aeroponics_Tank::Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::Aeroponics_TankSettings *DefaultSettings, PressurePump *Pump, PressureSensor *FeedbackPressureSensor) : Common(Name)
{ ///< constructor
  this->Parent = Parent;
  this->FeedbackPressureSensor = FeedbackPressureSensor;
  this->Pump = Pump;
  SprayEnabled = &DefaultSettings->SprayEnabled;   ///< Enable/disable misting
  Duration = &DefaultSettings->Duration;           ///< Spray time in seconds
  DayInterval = &DefaultSettings->DayInterval;     ///< Spray every X minutes - With lights ON
  NightInterval = &DefaultSettings->NightInterval; ///< Spray every X minutes - With lights OFF
  MinPressure = &DefaultSettings->MinPressure;     ///< Aeroponics - Turn on pump below this pressure (bar)
  MaxPressure = &DefaultSettings->MaxPressure;     ///< Aeroponics - Turn off pump above this pressure (bar)
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
  strcat_P(LongMessage, (PGM_P)F("Spray Solenoid:"));
  strcat(LongMessage, SpraySwitch->getStateText());
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
  if (State == AeroTankStates::SPRAYING)
    updateState(State);
}

void Aeroponics_Tank::updateState(AeroTankStates NewState) ///< Without a parameter actualize the current State. When NewState parameter is passed it overwrites State
{
  if (State != NewState)
  {
    State = NewState;
  }

  switch (State)
  {
  case AeroTankStates::DISABLED:

    break;
  case AeroTankStates::IDLE:
    if (Pump->getState() == PressurePumpStates::IDLE && FeedbackPressureSensor->getPressure() <= *MinPressure)
    { ///< If there is no spray in progress AND the pump is idle AND the pressure is below the minimum
      logToSerials(F("Tank recharging"), false, 3);
      Pump->startPump(false);
      updateState(AeroTankStates::REFILLING);
    }
    if (*SprayEnabled && (Pump->getState() == PressurePumpStates::IDLE || Pump->getState() == PressurePumpStates::DISABLED))
    {
      uint32_t Interval;
      if (DayMode)
      {
        Interval = *DayInterval * 60000; ///< Interval is in minutes
      }
      else
      {
        Interval = *NightInterval * 60000; ///< Interval is in minutes
      }
      if (millis() - SprayTimer >= Interval)
      { ///< if time to start spraying (AeroInterval in Minutes)
        sprayNow(false);
      }
    }
    break;
  case AeroTankStates::SPRAYING:
    if (millis() - SprayTimer >= *Duration * 1000)
    { ///< if time to stop spraying
      LastSprayPressure = Aeroponics_Tank::FeedbackPressureSensor->getPressure();
      sprayOff(false);
      updateState(AeroTankStates::RELEASEPRESSURE);
    }
    break;
  case AeroTankStates::RELEASEPRESSURE:
    if (Pump->getState() != PressurePumpStates::BLOWOFF) ///< Wait until pump finishes blowing off pressure
    {
      updateState(AeroTankStates::IDLE);
    }
    break;
  case AeroTankStates::REFILLING:
    if (Pump->getState() == PressurePumpStates::RUNNING && FeedbackPressureSensor->readPressure(false) >= *MaxPressure)
    { ///< refill complete, target pressure reached
      logToSerials(F("Tank recharged"), false, 3);
      Pump->stopPump();
      updateState(AeroTankStates::RELEASEPRESSURE);
    }
    if (Pump->getState() == PressurePumpStates::DISABLED) ///< if pump is on
    {                                                     ///< refill failed, target pressure was not reached before the pump timeout was reached
      logToSerials(F("Recharge failed"), false, 3);
      updateState(AeroTankStates::RELEASEPRESSURE);
    }
    break;
  case AeroTankStates::MIXING:

    break;
  case AeroTankStates::DRAINING:

    break;
  }
}

void Aeroponics_Tank::sprayNow(bool UserRequest)
{
  if (Pump->getState() == PressurePumpStates::IDLE || Pump->getState() == PressurePumpStates::DISABLED) //Only allow spraying when pump and bypass solenoid is off
  {
    SpraySwitch->turnOn();
    SprayTimer = millis();
    updateState(AeroTankStates::SPRAYING);

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
  SpraySwitch->turnOff();
  SprayTimer = millis();
  delay(*SpraySolenoidClosingDelay); ///< Wait until the spray solenoid is closed
  Pump->startBlowOff();              //< The pump's bypass is used to release pressure from the misting loop
  if (UserRequest)
  {
    Parent->getSoundObject()->playOffSound();
    Parent->addToLog(F("Spray OFF"));
  }
  else
  {
    logToSerials(F("Stopping spray"), true, 3);
  }
}

void Aeroponics_Tank::refillTank()
{
  Parent->addToLog(F("Refilling tank"));
  Pump->startPump(true);
}

/*
void Aeroponics_Tank::drainTank()
{
  Parent->addToLog(F("Draining tank"));
  SpraySwitch->turnOn();
  SprayTimer = millis();
  Pump->startBlowOff(); 
}
*/


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

void Aeroponics_Tank::setDayInterval(int Interval)
{
  if (*DayInterval != Interval && Interval > 0)
  {
    *DayInterval = Interval;
    Parent->getSoundObject()->playOnSound();
  }
}

int Aeroponics_Tank::getDayInterval()
{
  return *DayInterval;
}

char *Aeroponics_Tank::getDayIntervalText()
{
  return toText(*DayInterval);
}

void Aeroponics_Tank::setNightInterval(int Interval)
{
  if (*NightInterval != Interval && Interval > 0)
  {
    *NightInterval = Interval;
    Parent->getSoundObject()->playOnSound();
  }
}

int Aeroponics_Tank::getNightInterval()
{
  return *NightInterval;
}

char *Aeroponics_Tank::getNightIntervalText()
{
  return toText(*NightInterval);
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
    Parent->addToLog(F("Spray enabled"));
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    Parent->addToLog(F("Spray disabled"));
    Parent->getSoundObject()->playOffSound();
  }
}

bool Aeroponics_Tank::getSprayEnabled()
{
  return *SprayEnabled;
}

float Aeroponics_Tank::getDuration()
{
  return *Duration;
}

char *Aeroponics_Tank::getSprayDurationText()
{
  return toText(*Duration);
}

char *Aeroponics_Tank::sprayStateToText()
{
  return toText_onOff(*SprayEnabled);
}

float Aeroponics_Tank::getLastSprayPressure()
{
  return LastSprayPressure;
}

char *Aeroponics_Tank::getLastSprayPressureText(bool IncludeCurrentPressure)
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

float Aeroponics_Tank::getPressure()
{
  return FeedbackPressureSensor->getPressure();
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
    Parent->addToLog(F("Max pressure updated"));
    Parent->getSoundObject()->playOnSound();
  }
}