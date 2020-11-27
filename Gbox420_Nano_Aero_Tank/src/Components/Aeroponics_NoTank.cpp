#include "Aeroponics_NoTank.h"

Aeroponics_NoTank::Aeroponics_NoTank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) : Aeroponics(Name, Parent, DefaultSettings, FeedbackPressureSensor, Pump)
{
  this->Name = Name;
  logToSerials(F(""), true, 0);  //New line
  logToSerials(F(""), false, 1); //Extra indentation
  logToSerials(F("Aeroponics_NoTank object created"), true, 1);
  sprayNow(false); ///This is a safety feature,start with a spray after a reset
}

void Aeroponics_NoTank::refresh_Sec()
{ ///pump directly connected to aeroponics tote, with an electronically controlled bypass valve
  if (*Debug)
    Common::refresh_Sec();

  if (Pump->getState() == RUNNING)
  { ///if pump is on
    FeedbackPressureSensor->readPressure();
    if (FeedbackPressureSensor->getPressure() > *MaxPressure)
    {
      logToSerials(F("Max pressure reached"), false, 3);
      Pump->stopPump();
    }

    uint32_t Duration;
    if (DayMode)
    {
      Duration = *DayDuration * 1000; ///Duration is miliseconds, DayDuration in seconds
    }
    else
    {
      Duration = *NightDuration * 1000; ///Duration is miliseconds
    }

    if (!RunTillTimeout && (millis() - SprayTimer) >= Duration + (uint32_t)Pump->getPrimingTime() * 1000)
    { ///bypass valve is closed and time to stop spraying
      LastSprayPressure = Aeroponics::FeedbackPressureSensor->getPressure();
      logToSerials(F("Spray finished"), false, 3);
      Pump->stopPump();
    }
  }
  else
  {
    if (Pump->getState() == IDLE)
    {
      RunTillTimeout = false; ///Making sure the special flag is disabled as soon the Pump becomes IDLE. (RunTillTimeout forces the pump to run until the timeout is reached, used during mixing)
      uint32_t Interval;
      if (DayMode)
      {
        Interval = *DayInterval * 60000; ///Duration is miliseconds, DayInterval is Minutes
      }
      else
      {
        Interval = *NightInterval * 60000; ///Duration is miliseconds
      }
      if (millis() - SprayTimer >= Interval)
      { ///if time to start spraying
        sprayNow(false);
        SprayTimer = millis();
      }
    }
  }
}

void Aeroponics_NoTank::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("LastSprayPressure:"));
  strcat(LongMessage, toText_pressure(LastSprayPressure));
  logToSerials(&LongMessage, false, 1); ///first print Aeroponics_Tank specific report, without a line break
  Aeroponics::report();                 ///then print parent class report
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
