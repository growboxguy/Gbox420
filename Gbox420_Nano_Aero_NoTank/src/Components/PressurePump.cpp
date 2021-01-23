#include "PressurePump.h"

PressurePump::PressurePump(const __FlashStringHelper *Name, Module *Parent, Settings::PressurePumpSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  logToSerials(F(""), true, 0);  //New line
  logToSerials(F(""), false, 1); //Extra indentation
  PumpSwitch = new Switch_PWM(F("SpraySolenoid"), DefaultSettings->PumpPin, &DefaultSettings->Speed, &DefaultSettings->SpeedLowLimit, DefaultSettings->PumpPinNegativeLogic);
  if (DefaultSettings->BypassSolenoidPin != 255) //TODO: Split PressurePump into: PressurePump and PressurePump (inherits from PressurePump)
  {
    logToSerials(F(""), false, 1); //Extra indentation
    BypassSwitch = new Switch(F("BypassSolenoid"), DefaultSettings->BypassSolenoidPin, DefaultSettings->BypassSolenoidNegativeLogic);
  }
  BypassSolenoidClosingDelay = &DefaultSettings->BypassSolenoidClosingDelay;
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

  if (DefaultSettings->PrimingTime != -1)
  {
    PrimingTime = &DefaultSettings->PrimingTime;
  }
  if (DefaultSettings->BlowOffTime != -1)
  {
    BlowOffTime = &DefaultSettings->BlowOffTime;
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
      logToSerials(F("Priming complete, running..."), true, 3);
      updateState(PressurePumpStates::RUNNING);
    }
    break;
  case PressurePumpStates::RUNNING:
    PumpSwitch->turnOn();
    BypassSwitch->turnOff();
    *PumpEnabled = true;
    if (RunTime > 0 && millis() - PumpTimer > ((uint32_t)RunTime * 1000))
    {
      RunTime = 0;
      logToSerials(F("Running complete, stopping..."), true, 3);
      if (BlowOffTime != NULL && *BlowOffTime > 0)
      {
        updateState(PressurePumpStates::BLOWOFF);
      }
      else
      {
        updateState(PressurePumpStates::IDLE);
      }
    }
    if (millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000)) ///< Safety feature, During normal operation this should never be reached. The caller that turned on the pump should stop it before timeout is reached
    {
      Parent->addToLog(F("ALERT: Pump timeout reached"), 3); ///< \todo send email alert
      updateState(PressurePumpStates::DISABLED);
    }
    break;
  case PressurePumpStates::BLOWOFF:
    PumpSwitch->turnOff();
    BypassSwitch->turnOn();
    if (millis() - PumpTimer > ((uint32_t)*BlowOffTime * 1000)) ///< Is it time to disable the Bypass solenoid
    {
      logToSerials(F("Pressure released"), true, 3);
      if (BypassSolenoidClosingDelay != NULL && *BypassSolenoidClosingDelay > 0)
      {
        updateState(PressurePumpStates::CLOSINGBYPASS);
      }
      else
      {
        if (*PumpEnabled)
          updateState(PressurePumpStates::IDLE);
        else
          updateState(PressurePumpStates::DISABLED);
      }
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
  case PressurePumpStates::MIXING:
    PumpSwitch->turnOn();
    BypassSwitch->turnOn();
    *PumpEnabled = true;
    if ((RunTime > 0 && millis() - PumpTimer > ((uint32_t)RunTime * 1000)) || millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000))
    {
      RunTime = 0;
      logToSerials(F("Mixing finished"), true, 3);
      if (BypassSolenoidClosingDelay != NULL && *BypassSolenoidClosingDelay > 0)
      {
        updateState(PressurePumpStates::CLOSINGBYPASS);
      }
      else
      {
        updateState(PressurePumpStates::IDLE);
      }
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
    if (PrimingTime != NULL && *PrimingTime > 0)
    {
      updateState(PressurePumpStates::PRIMING);
    }
    else
    {
      updateState(PressurePumpStates::RUNNING);
    }
  }
  else
    logToSerials(F("disabled, cannot turn ON"), true, 1);
}

void PressurePump::stopPump()
{
  logToSerials(Name, false, 3);
  logToSerials(F("OFF"), true, 1);
  Parent->getSoundObject()->playOffSound();
  if (BlowOffTime != NULL && *BlowOffTime > 0)
  {
    updateState(PressurePumpStates::BLOWOFF);
  }
  else
  {
    updateState(PressurePumpStates::IDLE);
  }
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
  logToSerials(F("Pressure blow off"), true, 3);
  updateState(PressurePumpStates::BLOWOFF);
}

void PressurePump::setSpeed(uint8_t DutyCycle) //Set PWM duty cycle
{
  PumpSwitch->setDutyCycle(DutyCycle);
}

void PressurePump::startMixing(int TimeOutSec) ///< Mix the nutrient reservoir by turning on the bypass solenoid and the pump. Runs till the TimeOutSec parameter or the pump timeout
{
  if (TimeOutSec > 0)
  {
    RunTime = TimeOutSec;
  }
  else
  {
    RunTime = 0; ///< if no mix timeout defined -> Run until pump timeout is reached
  }
  Parent->addToLog(F("Mixing nutrients"));
  Parent->getSoundObject()->playOnSound();
  updateState(PressurePumpStates::MIXING);
}

void PressurePump::turnBypassOn()
{
  Parent->addToLog(F("Bypass ON"));
  Parent->getSoundObject()->playOnSound();
  updateState(PressurePumpStates::BLOWOFF);
}

void PressurePump::turnBypassOff()
{
  Parent->addToLog(F("Bypass OFF"));
  Parent->getSoundObject()->playOffSound();
  if (*PumpEnabled)
  {
    updateState(PressurePumpStates::IDLE);
  }
  else
  {
    updateState(PressurePumpStates::DISABLED);
  }
}

PressurePumpStates PressurePump::getState()
{
  return State;
}

char *PressurePump::getStateText()
{
  return toText_pressurePumpState(State);
}

bool PressurePump::getOnState()
{
  return PumpOn;
}

bool PressurePump::getBypassOnState()
{
  return BypassOn;
}

char *PressurePump::getBypassOnStateText()
{
  return toText_onOff(BypassOn);
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
    Parent->addToLog(F("Aero priming time updated"));
    Parent->getSoundObject()->playOnSound();
  }
}