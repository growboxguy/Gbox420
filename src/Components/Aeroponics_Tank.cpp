#include "Aeroponics_Tank.h"

Aeroponics_Tank::Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) : Aeroponics(Name, Parent, DefaultSettings, FeedbackPressureSensor, Pump)
{ ///constructor
  this->Name = Name;
  MinPressure = &TankSpecificSettings->MinPressure;   ///Aeroponics - Turn on pump below this pressure (bar)  
  SpraySwitch = new Switch(F("SpraySolenoid"),TankSpecificSettings->SpraySolenoidPin,TankSpecificSettings->SpraySolenoidNegativeLogic);
   
  logToSerials(F("Aeroponics_Tank object created"), true, 1);
  sprayNow(false); ///This is a safety feature,start with a spray after a reset
}

void Aeroponics_Tank::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Spray Solenoid:"));
  strcat(LongMessage, SpraySwitch -> getStateText());
  strcat_P(LongMessage, (PGM_P)F(" ; MinPressure:"));
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
    FeedbackPressureSensor->readPressure();
    if (Aeroponics::FeedbackPressureSensor->getPressure() >= *MaxPressure)
    { ///refill complete, target pressure reached      
      logToSerials(F("Pressure tank recharged"), false, 3);
      Pump-> stopPump();
    }   
  }
  else{
    if (!SpraySwitch -> getState() && Pump->getState() == IDLE && Aeroponics::FeedbackPressureSensor->getPressure() <= *MinPressure)
    {                    
      logToSerials(F("Pressure tank recharging..."), false, 3);
      Pump->startPump();
    }
  }
    
  if (SpraySwitch -> getState())
  { ///if spray is on
    if (millis() - SprayTimer >= ((uint32_t)*Duration * 1000))
    { ///if time to stop spraying (Duration in Seconds)
      LastSprayPressure = Aeroponics::FeedbackPressureSensor->getPressure();
      sprayOff(false);
    }
  }
  else
  { ///if spray is off
    if (*SprayEnabled && millis() - SprayTimer >= ((uint32_t)*Interval * 60000))
    { ///if time to start spraying (AeroInterval in Minutes)
      sprayNow(false);
    }
  }  
}



void Aeroponics_Tank::sprayNow(bool UserRequest)
{ 
  SpraySwitch -> turnOn();
  SprayTimer = millis();
  Parent->getSoundObject()->playOnSound();
  if (UserRequest)
  {
    Parent->addToLog(F("Aeroponics spraying"));
  }
  else
  {
    logToSerials(F("Aeroponics spraying"), true, 3);
  }
}

void Aeroponics_Tank::sprayOff(bool UserRequest)
{
  SpraySwitch -> turnOff();
  SprayTimer = millis();
  Parent->getSoundObject()->playOffSound();
  if(UserRequest)
  {
    Parent->addToLog(F("Aeroponics spray OFF"));
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