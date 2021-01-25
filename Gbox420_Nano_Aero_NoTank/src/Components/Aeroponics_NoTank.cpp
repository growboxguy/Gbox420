#include "Aeroponics_NoTank.h"

Aeroponics_NoTank::Aeroponics_NoTank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, PressurePump *Pump) : Common(Name)
{
  this->Parent = Parent;
  this->FeedbackPressureSensor = FeedbackPressureSensor;
  this->Pump = Pump;
  SprayEnabled = &DefaultSettings->SprayEnabled;   ///< Enable/disable misting
  Duration = &DefaultSettings->Duration;     ///< Spray time in seconds
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
  logToSerials(&LongMessage, true, 1); ///< Break line, No indentation needed: child class already printed it


}

void Aeroponics_NoTank::refresh_Sec()
{ ///< pump directly connected to aeroponics tote, with an electronically controlled bypass valve
  if (*Debug)
    Common::refresh_Sec();
  checkPump(false);
}

void Aeroponics_NoTank::processTimeCriticalStuff()
{
  checkPump(true);
}

void Aeroponics_NoTank::checkPump(bool OnlyTurnOff)
{
  if (Pump->getState() == PressurePumpStates::RUNNING)
  { ///< if pump is on
    FeedbackPressureSensor->readPressure();
    if (FeedbackPressureSensor->getPressure() > *MaxPressure)
    {
      logToSerials(F("Max pressure reached"), false, 3);
      Pump->stopPump();
    }

    if (!RunTillTimeout && (millis() - SprayTimer) >= *Duration * 1000 + (uint32_t)Pump->getPrimingTime() * 1000)
    { ///< bypass valve is closed and time to stop spraying
      LastSprayPressure = FeedbackPressureSensor->getPressure();
      logToSerials(F("Spray finished"), false, 3);
      Pump->stopPump();
    }
  }
  else
  {
    if (!OnlyTurnOff && Pump->getState() == PressurePumpStates::IDLE)
    {
      RunTillTimeout = false; ///< Making sure the special flag is disabled as soon the Pump becomes IDLE. (RunTillTimeout forces the pump to run until the timeout is reached, used during mixing)
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
        sprayNow(false);
        SprayTimer = millis();
      }
    }
  }
}

void Aeroponics_NoTank::sprayNow(bool UserRequest)
{
  if (*SprayEnabled || UserRequest)
  {
    Parent->getSoundObject()->playOnSound();
    if (UserRequest)
      Parent->addToLog(F("Aeroponics spraying"));
    else
      logToSerials(F("Aeroponics spraying"), false, 3);
    Pump->startPump(UserRequest);
    SprayTimer = millis();
  }
}

void Aeroponics_NoTank::sprayOff()
{
  Parent->addToLog(F("Aeroponics spray OFF"));
  Pump->stopPump();
}

void Aeroponics_NoTank::lockPumpOn()
{
  Pump->startPump(true);
  RunTillTimeout = true;
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
    Parent->addToLog(F("Spray enabled"));
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    Parent->addToLog(F("Spray disabled"));
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

char *Aeroponics_NoTank::getLastSprayPressureText(bool IncludeCurrentPressure)
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