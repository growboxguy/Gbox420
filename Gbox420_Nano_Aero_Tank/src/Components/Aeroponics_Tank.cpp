#include "Aeroponics_Tank.h"

Aeroponics_Tank::Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) : Aeroponics(Name, Parent, DefaultSettings, FeedbackPressureSensor, Pump)
{ ///< constructor
  this->Name = Name;
  MinPressure = &TankSpecificSettings->MinPressure; ///< Aeroponics - Turn on pump below this pressure (bar)
  logToSerials(F(""), true, 0);                     //New line
  logToSerials(F(""), false, 1);                    //Extra indentation
  SpraySwitch = new Switch(F("SpraySolenoid"), TankSpecificSettings->SpraySolenoidPin, TankSpecificSettings->SpraySolenoidNegativeLogic);

  logToSerials(F("Aeroponics_Tank object created"), true, 1);
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
  logToSerials(&LongMessage, false, 1); ///< first print Aeroponics_Tank specific report, without a line break
  Aeroponics::report();                 ///< then print parent class report
}

void Aeroponics_Tank::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  checkPump();
  checkSpray(false);
}

void Aeroponics_Tank::processTimeCriticalStuff(){
  checkSpray(true);
}

void Aeroponics_Tank::checkPump()
{
  if (Pump->getState() == RUNNING) ///< if pump is on
  {
    FeedbackPressureSensor->readPressure();
    if (Aeroponics::FeedbackPressureSensor->getPressure() >= *MaxPressure)
    { ///< refill complete, target pressure reached
      logToSerials(F("Pressure tank recharged"), false, 3);
      Pump->stopPump();
    }
  }
  else
  {
    if (!SpraySwitch->getState() && Pump->getState() == IDLE && Aeroponics::FeedbackPressureSensor->getPressure() <= *MinPressure)
    { ///< If there is no spray in progress AND the pump is idle AND the pressure is below the minimum
      logToSerials(F("Pressure tank recharging..."), false, 3);
      Pump->startPump();
    }
  }
}

void Aeroponics_Tank::checkSpray(bool OnlyTurnOff)
{
  if (SpraySwitch->getState())
  { ///< if spray is on
    if (millis() - SprayTimer >= *Duration * 1000)
    { ///< if time to stop spraying
      LastSprayPressure = Aeroponics::FeedbackPressureSensor->getPressure();
      sprayOff(false);
    }
  }
  else
  { ///< if spray is off
    if (!OnlyTurnOff)
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

      if (*SprayEnabled && millis() - SprayTimer >= Interval)
      { ///< if time to start spraying (AeroInterval in Minutes)
        sprayNow(false);
      }
    }
  }
}

void Aeroponics_Tank::sprayNow(bool UserRequest)
{
  Pump->turnBypassOff();
  SpraySwitch->turnOn();
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
  SpraySwitch->turnOff();
  SprayTimer = millis();
  delay(400); ///< Wait 0.4sec for the spray solenoid to close
  Pump->startBlowOff(); //< The pump's bypass is used to release pressure from the misting loop: Stops spraying immediately
  if (UserRequest)
  {
    Parent->getSoundObject()->playOffSound();
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