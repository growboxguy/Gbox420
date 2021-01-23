#include "Aeroponics_Tank.h"

Aeroponics_Tank::Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor, PressurePump *Pump) : Aeroponics(Name, Parent, DefaultSettings, FeedbackPressureSensor, Pump)
{ ///< constructor
  this->Name = Name;
  MinPressure = &TankSpecificSettings->MinPressure; ///< Aeroponics - Turn on pump below this pressure (bar)
  SpraySolenoidClosingDelay = &TankSpecificSettings->SpraySolenoidClosingDelay;
  logToSerials(F(""), true, 0);  //New line
  logToSerials(F(""), false, 1); //Extra indentation
  SpraySwitch = new Switch(F("SpraySolenoid"), TankSpecificSettings->SpraySolenoidPin, TankSpecificSettings->SpraySolenoidNegativeLogic);

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

void Aeroponics_Tank::processTimeCriticalStuff()
{
  checkSpray(true);
}

void Aeroponics_Tank::checkPump()
{
  if (Pump->getState() == PressurePumpStates::RUNNING) ///< if pump is on
  {
    if (FeedbackPressureSensor->readPressure(false) >= *MaxPressure)
    { ///< refill complete, target pressure reached
      logToSerials(F("Tank recharged"), false, 3);
      Pump->stopPump();
    }
  }
  else
  {
    if (!SpraySwitch->getState() && Pump->getState() == PressurePumpStates::IDLE && FeedbackPressureSensor->getPressure() <= *MinPressure)
    { ///< If there is no spray in progress AND the pump is idle AND the pressure is below the minimum
      logToSerials(F("Tank recharging"), false, 3);
      Pump->startPump(false);
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
    if (!OnlyTurnOff && (Pump->getState() == PressurePumpStates::IDLE || Pump->getState() == PressurePumpStates::DISABLED))
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
  if (Pump->getState() == PressurePumpStates::IDLE || Pump->getState() == PressurePumpStates::DISABLED)  //Only allow spraying when pump and bypass solenoid is off
  {
    SpraySwitch->turnOn();
    SprayTimer = millis();
    Parent->getSoundObject()->playOnSound();
    if (UserRequest)
    {
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