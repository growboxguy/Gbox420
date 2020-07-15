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

  if (Pump->getState() == RUNNING)
  { ///if pump is on    
    FeedbackPressureSensor->readPressure();
    if(FeedbackPressureSensor->getPressure() > *MaxPressure){
      logToSerials(F("Max pressure reached"), false, 3);
      Pump -> stopPump();
    }
    
    if (!LockedPumpOn && (millis() - SprayTimer) >= ((uint32_t)*Duration * 1000 + (uint32_t)Pump->getPrimingTime() * 1000))
    { ///bypass valve is closed and time to stop spraying (Duration in Seconds)
      LastSprayPressure = Aeroponics::FeedbackPressureSensor->getPressure();
      logToSerials(F("Spray finished"), false, 3);
      Pump -> stopPump(); 
    }    
  }
  else
  {    
    if (Pump->getState() == IDLE)  
    { 
      LockedPumpOn = false;  ///Release the pump ON lock after pump returns to idle state after reaching PumpTimeOut

      if(millis() - SprayTimer >= (uint32_t)*Interval * 60000){  ///if time to start spraying (AeroInterval in Minutes)
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
  Parent -> addToLog(F("Aeroponics spray OFF"));
  Pump -> stopPump();  
}

void Aeroponics_NoTank::lockPumpOn(){
  Pump -> startPump(true);
  LockedPumpOn = true;
}
