#include "Aeroponics_NoTank.h"

Aeroponics_NoTank::Aeroponics_NoTank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) : Aeroponics(Name, Parent, DefaultSettings, FeedbackPressureSensor, Pump)
{
  this->Name = Name;
  logToSerials(F("Aeroponics_NoTank object created"), true, 1);
  sprayNow(false); ///This is a safety feature,start with a spray after a reset
}

void Aeroponics_NoTank::refresh_Sec()
{ ///pump directly connected to aeroponics tote, with an electronically controlled bypass valve
  if (*Debug)
    Common::refresh_Sec();

  if (BlowOffInProgress && millis() - SprayTimer >= ((uint32_t)*BlowOffTime * 1000))
  {                           ///checking pressure blow-off timeout
    setBypassOff(false);
    BlowOffInProgress = false;
    logToSerials(F("Stopping blow-off"), true);
    Aeroponics::FeedbackPressureSensor->readPressure(); ///Update pressure after closing bypass valve
  }
  if (Pump->getOnState())
  { ///if pump is on
    FeedbackPressureSensor->readPressure();
    
    if (!MixInProgress && !BypassOn && Pump->getOnState() && millis() - SprayTimer >= ((uint32_t)*Duration * 1000))
    { ///bypass valve is closed and time to stop spraying (Duration in Seconds)
      LastSprayPressure = Aeroponics::FeedbackPressureSensor->getPressure();
      Pump -> stopPump(false);
      BlowOffInProgress = true; ///no extra timer is needed, will use SprayTimer
      SprayTimer = millis();      
      logToSerials(F("Stopping spray"), true);
    }
    else
    {
      if (!MixInProgress && BypassOn && millis() - Pump->PumpTimer >= ((uint32_t)*PrimingTime * 1000))
      {                           ///self priming timeout reached, time to start spraying
        setBypassOff(false);
        SprayTimer = millis();
        Aeroponics::FeedbackPressureSensor->Pressure->resetAverage();
        logToSerials(F("Closing bypass, starting spray"), true);
      }
    }
  }
  else
  { ///pump is off
    if (!BlowOffInProgress)
      BypassOn = false; ///Should not leave the solenoid turned on
    if (Pump -> getEnabledState() && *SprayEnabled && ((millis() - SprayTimer) >= ((uint32_t)*Interval * 60000)))
    { ///if time to start spraying (AeroInterval in Minutes)
      sprayNow(false);
    }
  }
}

void Aeroponics_NoTank::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("LastSprayPressure:"));
  strcat(LongMessage, pressureToText(LastSprayPressure));
  strcat_P(LongMessage, (PGM_P)F(" ; BlowOffTime:"));
  strcat(LongMessage, toText(*BlowOffTime));
  logToSerials(&LongMessage, false, 1); ///first print Aeroponics_Tank specific report, without a line break
  Aeroponics::report();                 ///then print parent class report
}

void Aeroponics_NoTank::sprayNow(bool UserRequest)
{
  if (*SprayEnabled || UserRequest)
  {
    MixInProgress = false;
    setBypassOn(false);
    Pump->startPump(UserRequest);
    SprayTimer = millis();
    Parent->getSoundObject()->playOnSound();
    if (UserRequest)
      Parent->addToLog(F("Aeroponics spraying"));
    else
      logToSerials(F("Aeroponics spraying"), true, 3);
  }
}

void Aeroponics_NoTank::sprayOff(bool UserRequest)
{
  Pump -> stopPump(UserRequest);
  setBypassOff(false);
  Parent->addToLog(F("Aeroponics spray OFF"));
}

char *Aeroponics_NoTank::sprayStateToText()
{
  if (!*SprayEnabled)
    return (char *)"DISABLED";
  else
  {
    if (Pump->getOnState() && !BypassOn)
      return (char *)"ON";
    else
      return (char *)"ENABLED";
  }
}
