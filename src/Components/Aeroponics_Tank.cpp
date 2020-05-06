#include "Aeroponics_Tank.h"

Aeroponics_Tank::Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) : Aeroponics(Name, Parent, DefaultSettings, FeedbackPressureSensor, Pump)
{ ///constructor
  this->Name = Name;
  SpraySolenoidPin = &TankSpecificSettings->SpraySolenoidPin;
  MinPressure = &TankSpecificSettings->MinPressure;   ///Aeroponics - Turn on pump below this pressure (bar)  
  pinMode(*SpraySolenoidPin, OUTPUT);
  digitalWrite(*SpraySolenoidPin, HIGH); ///initialize off
  logToSerials(F("Aeroponics_Tank object created"), true, 1);
  sprayNow(false); ///This is a safety feature,start with a spray after a reset
}

void Aeroponics_Tank::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Low Pressure:"));
  strcat(LongMessage, toText_pressure(*MinPressure));
  logToSerials(&LongMessage, false, 1); ///first print Aeroponics_Tank specific report, without a line break
  Aeroponics::report();                 ///then print parent class report
}

void Aeroponics_Tank::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();  

  if (Pump->getState() == RUNNING) ///< if pump is on
  { 
    if (Aeroponics::FeedbackPressureSensor->getPressure() >= *MaxPressure)
    { ///refill complete, target pressure reached
      Pump-> stopPump();
      logToSerials(F("Pressure tank recharged"), true);
    }   
  }
  else{
    if (Pump->getState() == IDLE && Aeroponics::FeedbackPressureSensor->getPressure() <= *MinPressure)
    {                    
      logToSerials(F("Pressure tank recharging..."), true);
      Pump->startPump(false);
    }
  }
    
  if (SpraySolenoidOn)
  { ///if spray is on
    if (millis() - SprayTimer >= ((uint32_t)*Duration * 1000))
    { ///if time to stop spraying (Duration in Seconds)
      LastSprayPressure = Aeroponics::FeedbackPressureSensor->getPressure();
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

  if(SpraySolenoidOn){ 
    digitalWrite(*SpraySolenoidPin, LOW);  
  }
  else{  
    digitalWrite(*SpraySolenoidPin, HIGH);  
  }
}



void Aeroponics_Tank::sprayNow(bool UserRequest)
{
  if (*SprayEnabled || UserRequest)
  {    
    SprayTimer = millis();
    Pump -> startPump(UserRequest);
    Parent->getSoundObject()->playOnSound();
    if (UserRequest)
      Parent->addToLog(F("Aeroponics spraying"));
    else
      logToSerials(F("Aeroponics spraying"), true, 3);
  }
}

void Aeroponics_Tank::sprayOff(bool UserRequest)
{
  Pump -> stopPump(UserRequest);
  if(UserRequest)
  {
    Parent->addToLog(F("Aeroponics spray OFF"));
  }
}

void Aeroponics_Tank::refillTank()
{
  Parent->addToLog(F("Refilling tank"));
  Pump->startPump();
}