#include "PressurePump.h"

PressurePump::PressurePump(const __FlashStringHelper *Name, Module *Parent, Settings::PressurePumpSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  logToSerials(F(""), true, 0);  //New line
  logToSerials(F(""), false, 1); //Extra indentation
  PumpSwitch = new Switch_PWM(F("SpraySolenoid"), DefaultSettings->PumpPin, &DefaultSettings->Speed, &DefaultSettings->SpeedLowLimit, DefaultSettings->PumpPinNegativeLogic);
  logToSerials(F(""), false, 1); //Extra indentation
  BypassSwitch = new Switch(F("BypassSolenoid"), DefaultSettings->BypassSolenoidPin, DefaultSettings->BypassSolenoidNegativeLogic);
  BypassSolenoidMaxOpenTime = &DefaultSettings->BypassSolenoidMaxOpenTime;
  BypassSolenoidClosingDelay = &DefaultSettings->BypassSolenoidClosingDelay;
  PrimingTime = &DefaultSettings->PrimingTime;
  BlowOffTime = &DefaultSettings->BlowOffTime;
  PumpTimeOut = &DefaultSettings->PumpTimeOut;
  PumpEnabled = &DefaultSettings->PumpEnabled;

  if (*PumpEnabled)
  {
    State = PressurePumpStates::IDLE;
  }
  else
  {
    State = PressurePumpStates::DISABLED;
  }
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  logToSerials(F("PressurePump object created"), true, 2);
}

void PressurePump::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("State:"));
  strcat(LongMessage, getStateText());
  strcat_P(LongMessage, (PGM_P)F(" ; TimeOut:"));
  strcat(LongMessage, toText_second(*PumpTimeOut));
  logToSerials(&LongMessage, true, 1);
}

void PressurePump::refresh_Sec()
{
  if (*Debug)
  {
    Common::refresh_Sec();
  }
  updateState(State);
}

void PressurePump::updateState(PressurePumpStates NewState) ///< Without a parameter actualize the current State. When NewState parameter is passed it overwrites State
{
  if (State != NewState) ///< if not the default value was passed
  {
    State = NewState;
    PumpTimer = millis(); ///< Start measuring the time spent in the new State
  }

  switch (State)
  {
  case PressurePumpStates::PRIMING:
    PumpSwitch->turnOn();
    BypassSwitch->turnOn();
    if (millis() - PumpTimer > ((uint32_t)*PrimingTime * 1000)) ///< Is it time to disable the Bypass solenoid
    {
      updateState(PressurePumpStates::RUNNING);
    }
    break;
  case PressurePumpStates::RUNNING:
    PumpSwitch->turnOn();
    BypassSwitch->turnOff();
    *PumpEnabled = true;
    if (millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000)) ///< Safety feature, During normal operation this should never be reached. The caller that turned on the pump should stop it before timeout is reached
    {
      Parent->addToLog(F("Pump timeout"), 3); ///< \todo send email alert
      updateState(PressurePumpStates::DISABLED);
    }
    break;
  case PressurePumpStates::BLOWOFF:
    PumpSwitch->turnOff();
    BypassSwitch->turnOn();
    if (millis() - PumpTimer > ((uint32_t)*BlowOffTime * 1000)) ///< Is it time to disable the Bypass solenoid
    {
      updateState(PressurePumpStates::CLOSINGBYPASS);
    }
    break;
  case PressurePumpStates::BYPASSOPEN:
    PumpSwitch->turnOff();
    BypassSwitch->turnOn();
    if (millis() - PumpTimer > ((uint32_t)*BypassSolenoidMaxOpenTime * 1000)) ///< Is it time to disable the Bypass solenoid
    {
      updateState(PressurePumpStates::CLOSINGBYPASS);
    }
    break;
  case PressurePumpStates::IDLE:
    PumpSwitch->turnOff();
    BypassSwitch->turnOff();
    *PumpEnabled = true;
    break;
  case PressurePumpStates::CLOSINGBYPASS:
    PumpSwitch->turnOff();
    BypassSwitch->turnOff();
    if (millis() - PumpTimer > ((uint32_t)*BypassSolenoidClosingDelay)) ///< Bypass is fully closed
    {
      if (*PumpEnabled)
        updateState(PressurePumpStates::IDLE);
      else
        updateState(PressurePumpStates::DISABLED);
    }
    break;
  case PressurePumpStates::MIX:
    PumpSwitch->turnOn();
    BypassSwitch->turnOn();
    *PumpEnabled = true;
    if (millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000))
    {
      updateState(PressurePumpStates::CLOSINGBYPASS);
    }
    break;
  case PressurePumpStates::DISABLED:
    PumpSwitch->turnOff();
    BypassSwitch->turnOff();
    *PumpEnabled = false;
    break;
  }
}

void PressurePump::startPump(bool ResetStatus)
{
  if (ResetStatus)
  {
    *PumpEnabled = true;
  }
  logToSerials(Name, false, 3);
  if (*PumpEnabled)
  {
    logToSerials(F("ON"), true, 1);
    Parent->getSoundObject()->playOnSound();
    updateState(PressurePumpStates::PRIMING);
  }
  else
    logToSerials(F("disabled"), true, 1);
}

void PressurePump::stopPump(bool ResetStatus)
{
  if (ResetStatus)
  {
    *PumpEnabled = true;
  }
  logToSerials(Name, false, 3);
  logToSerials(F("OFF"), true, 1);
  Parent->getSoundObject()->playOffSound();
  updateState(PressurePumpStates::BLOWOFF);
}

void PressurePump::disablePump()
{
  logToSerials(Name, false, 3);
  logToSerials(F("disabled"), true, 1);
  Parent->getSoundObject()->playOffSound();
  updateState(PressurePumpStates::DISABLED);
}

void PressurePump::startBlowOff()
{
  PumpTimer = millis();
  logToSerials(F("Blow off"), true, 3);
  updateState(PressurePumpStates::BLOWOFF);
}

void PressurePump::setSpeed(uint8_t DutyCycle) //Set PWM duty cycle
{
  PumpSwitch->setDutyCycle(DutyCycle);
}

void PressurePump::startMixing() ///< Mix the nutrient reservoir by turning on the bypass solenoid and the pump. Runs till the TimeOutSec parameter or the pump timeout
{
  
  Parent->getSoundObject()->playOnSound();
  updateState(PressurePumpStates::MIX);
}

void PressurePump::turnBypassOn(bool KeepOpen)
{
  Parent->addToLog(F("Bypass ON"));
  Parent->getSoundObject()->playOnSound();
  if (KeepOpen)
    updateState(PressurePumpStates::BYPASSOPEN);
  else
    updateState(PressurePumpStates::BLOWOFF);
}

void PressurePump::turnBypassOff()
{
  Parent->addToLog(F("Bypass OFF"));
  Parent->getSoundObject()->playOffSound();
  updateState(PressurePumpStates::CLOSINGBYPASS);
}

PressurePumpStates PressurePump::getState()
{
  return State;
}

char *PressurePump::getStateText()
{
  return toText_pressurePumpState(State);
}

bool PressurePump::getEnabledState()
{
  return *PumpEnabled;
}

int PressurePump::getPumpTimeOut()
{
  return *PumpTimeOut;
}

void PressurePump::setPumpTimeOut(int TimeOut)
{
  if (*this->PumpTimeOut != TimeOut && TimeOut > 0)
  {
    *this->PumpTimeOut = TimeOut;
    logToSerials(Name, false, 1);
    logToSerials(F("timeout updated"), true, 1);
    Parent->getSoundObject()->playOnSound();
  }
}

int PressurePump::getPrimingTime()
{
  return *PrimingTime;
}

void PressurePump::setPrimingTime(int Timing)
{
  if (*PrimingTime != Timing && Timing > 0)
  {
    *PrimingTime = Timing;
    Parent->addToLog(F("Priming time updated"));
    Parent->getSoundObject()->playOnSound();
  }
}