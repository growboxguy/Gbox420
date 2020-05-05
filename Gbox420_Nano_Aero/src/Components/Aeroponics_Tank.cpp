#include "Aeroponics_Tank.h"

Aeroponics_Tank::Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) : Aeroponics(Name, Parent, DefaultSettings, FeedbackPressureSensor, Pump)
{ ///constructor
  this->Name = Name;
  SpraySolenoidPin = &TankSpecificSettings->SpraySolenoidPin;
  LowPressure = &TankSpecificSettings->LowPressure;   ///Aeroponics - Turn on pump below this pressure (bar)  
  pinMode(*SpraySolenoidPin, OUTPUT);
  digitalWrite(*SpraySolenoidPin, HIGH); ///initialize off
  logToSerials(F("Aeroponics_Tank object created"), true, 1);
  sprayNow(false); ///This is a safety feature,start with a spray after a reset
}

void Aeroponics_Tank::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  if (Pump->getOnState()) ///< True if pump is on
  { 
    if (Aeroponics::FeedbackPressureSensor->getPressure() >= *HighPressure)
    { ///refill complete, target pressure reached
      stopPump(false);
      logToSerials(F("Pressure tank recharged"), true);
    }
    else
    {      
      if (!MixInProgress && BypassOn && millis() - Pump->PumpTimer >= ((uint32_t)*PrimingTime * 1000))
      { ///self priming timeout reached, time to start refilling
        logToSerials(F("Starting refill"), true);
        setBypassOff(false);
        Pump->PumpTimer = millis(); ///reset timer to start measuring refill time
      }
    }
  }
  else ///< If pump is off
  {                           
    BypassOn = false; ///Should not leave the solenoid turned on
    MixInProgress = false; ///Mixing state ends when pump timeout is reached and the pump is turned off

    if (Pump->getEnabledState() && Aeroponics::FeedbackPressureSensor->getPressure() <= *LowPressure)
    {
      if (*Debug)
        logToSerials(F("Starting bypass"), true);
      setBypassOn(false);
      Pump->startPump(false);
    }
  }
    
  if (SpraySolenoidOn)
  { ///if spray is on
    if (millis() - SprayTimer >= ((uint32_t)*Duration * 1000))
    { ///if time to stop spraying (Duration in Seconds)
      SpraySolenoidOn = false;
      logToSerials(F("Stopping spray"), true);
      Parent->getSoundObject()->playOffSound();
      SprayTimer = millis();
    }
  }
  else
  { ///if spray is off
    if (*SprayEnabled && millis() - SprayTimer >= ((uint32_t)*Interval * 60000))
    { ///if time to start spraying (AeroInterval in Minutes)
      SpraySolenoidOn = true;
      Parent->getSoundObject()->playOnSound();
      if (*Debug)
        logToSerials(F("Starting spray"), true);
      SprayTimer = millis();
    }
  }
}

void Aeroponics_Tank::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Pressure:"));
  strcat(LongMessage, FeedbackPressureSensor->getPressureText(true, false));
  logToSerials(&LongMessage, false, 1); ///first print Aeroponics_Tank specific report, without a line break
  Aeroponics::report();                 ///then print parent class report
}

void Aeroponics_Tank::setSpraySolenoidOn()
{ 
  digitalWrite(*SpraySolenoidPin, LOW);  
}

void Aeroponics_Tank::setSpraySolenoidOff()
{  
  digitalWrite(*SpraySolenoidPin, HIGH);  
}


void Aeroponics_Tank::setLowPressure(float LowPressure)
{
  *(this->LowPressure) = LowPressure;
}

void Aeroponics_Tank::setHighPressure(float HighPressure)
{
  *(this->HighPressure) = HighPressure;
  Parent->addToLog(F("Tank limits updated"));
}

void Aeroponics_Tank::sprayNow(bool UserRequest)
{
  if (*SprayEnabled || UserRequest)
  {
    MixInProgress = false;
    SprayTimer = millis();
    setSpraySolenoidOn();
    Parent->getSoundObject()->playOnSound();
    if (UserRequest)
      Parent->addToLog(F("Aeroponics spraying"));
    else
      logToSerials(F("Aeroponics spraying"), true, 3);
  }
}

void Aeroponics_Tank::sprayOff(bool UserRequest)
{
  setSpraySolenoidOff();
  if(UserRequest)
  {
    Parent->addToLog(F("Aeroponics spray OFF"));
  }
}

char *Aeroponics_Tank::sprayStateToText()
{
  if (!*SprayEnabled && !SpraySolenoidOn)
    return (char *)"DISABLED";
  else
  {
    if (SpraySolenoidOn)
      return (char *)"ON";
    else
      return (char *)"ENABLED";
  }
}

void Aeroponics_Tank::refillTank()
{
  Parent->addToLog(F("Refilling tank"));
  setBypassOn(false);
  Pump->startPump();
}