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
  logToSerials(F("PressurePump ready"), true, 3);
}

void PressurePump::report(bool JSONReport)
{
  Common::report(JSONReport); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
    strcat(LongMessage, getStateText());
    strcat_P(LongMessage, (PGM_P)F("\",\"T\":\""));
    strcat(LongMessage, toText_second(*PumpTimeOut));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else //Print a report to the Serial console
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("State:"));
    strcat(LongMessage, getStateText());
    strcat_P(LongMessage, (PGM_P)F(" ; TimeOut:"));
    strcat(LongMessage, toText_second(*PumpTimeOut));
    logToSerials(&LongMessage, true, 1);
  }
}

void PressurePump::refresh_Sec()
{
  if (*Debug)
  {
    Common::refresh_Sec();
  }
  updateState(State);
}

void PressurePump::updateState(PressurePumpStates NewState) ///< Actualize the current State
{
  bool BlockOverWritingState = false; //Used when a state transitions to a new state
  if (State != NewState)
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)Name);
    strcat_P(LongMessage, (PGM_P)F(" state"));
    strcat(LongMessage, toText_pressurePumpState(State));
    strcat_P(LongMessage, (PGM_P)F(" -> "));
    strcat(LongMessage, toText_pressurePumpState(NewState));
    logToSerials(&LongMessage, true, 3);

    PumpTimer = millis(); ///< Start measuring the time spent in the new State
  }

  switch (NewState)
  {
  case PressurePumpStates::IDLE:
    if (State != NewState)
    {
      PumpSwitch->turnOff();
      BypassSwitch->turnOff();
      *PumpEnabled = true;
    }
    break;
  case PressurePumpStates::PRIMING:
    if (State != NewState)
    {
      PumpSwitch->turnOn();
      BypassSwitch->turnOn();
    }
    if (millis() - PumpTimer > ((uint32_t)*PrimingTime * 1000)) ///< Is it time to disable the Bypass solenoid
    {
      updateState(PressurePumpStates::RUNNING);
      BlockOverWritingState = true;
    }
    break;
  case PressurePumpStates::RUNNING:
    if (State != NewState)
    {
      PumpSwitch->turnOn();
      BypassSwitch->turnOff();
      *PumpEnabled = true;
    }
    if (millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000)) ///< Safety feature, During normal operation this should never be reached. The caller that turned on the pump should stop it before timeout is reached
    {
      appendName(true);
      strcat_P(ShortMessage, (PGM_P)F("timeout"));
      Parent->addToLog(ShortMessage);
      *PumpEnabled = false;
      updateState(PressurePumpStates::BLOWOFF);
      BlockOverWritingState = true;
    }
    break;
  case PressurePumpStates::BLOWOFF:
    if (State != NewState)
    {
      PumpSwitch->turnOff();
      BypassSwitch->turnOn();
    }
    if (millis() - PumpTimer > ((uint32_t)*BlowOffTime * 1000)) ///< Is it time to disable the Bypass solenoid
    {
      updateState(PressurePumpStates::BYPASSCLOSE);
      BlockOverWritingState = true;
    }
    break;
  case PressurePumpStates::BYPASSOPEN:
    if (State != NewState)
    {
      PumpSwitch->turnOff();
      BypassSwitch->turnOn();
    }
    if (millis() - PumpTimer > ((uint32_t)*BypassSolenoidMaxOpenTime * 1000)) ///< Is it time to disable the Bypass solenoid
    {
      updateState(PressurePumpStates::BYPASSCLOSE);
      BlockOverWritingState = true;
    }
    break;
  case PressurePumpStates::BYPASSCLOSE:
    if (State != NewState)
    {
      PumpSwitch->turnOff();
      BypassSwitch->turnOff();
    }
    if (millis() - PumpTimer > ((uint32_t)*BypassSolenoidClosingDelay)) ///< Bypass is fully closed
    {
      if (*PumpEnabled)
        updateState(PressurePumpStates::IDLE);
      else
        updateState(PressurePumpStates::DISABLED);
      BlockOverWritingState = true;
    }
    break;
  case PressurePumpStates::MIX:
    if (State != NewState)
    {
      PumpSwitch->turnOn();
      BypassSwitch->turnOn();
    }
    *PumpEnabled = true;
    if (millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000))
    {
      updateState(PressurePumpStates::BYPASSCLOSE);
      BlockOverWritingState = true;
    }
    break;
  case PressurePumpStates::DISABLED:
    if (State != NewState)
    {
      PumpSwitch->turnOff();
      BypassSwitch->turnOff();
      *PumpEnabled = false;
    }
    break;
  }

  if (State != NewState && !BlockOverWritingState)
  {
    State = NewState;
  }
}

void PressurePump::startPump(bool ResetState)
{
  if (ResetState)
  {
    *PumpEnabled = true;
  }
  if (*PumpEnabled)
  {
    Parent->getSoundObject()->playOnSound();
    updateState(PressurePumpStates::PRIMING);
  }
}

void PressurePump::stopPump(bool ResetState)
{
  if (ResetState)
  {
    *PumpEnabled = true;
  }
  Parent->getSoundObject()->playOffSound();
  if (State == PressurePumpStates::RUNNING)
  {
    updateState(PressurePumpStates::BLOWOFF);
  }
  else
  {
    if ((State != PressurePumpStates::IDLE && State != PressurePumpStates::DISABLED) || (State == PressurePumpStates::DISABLED && ResetState))
      updateState(PressurePumpStates::BYPASSCLOSE);
  }
}

void PressurePump::disablePump()
{
  Parent->getSoundObject()->playOffSound();
  *PumpEnabled = false;
  if (State == PressurePumpStates::RUNNING)
  {
    updateState(PressurePumpStates::BLOWOFF);
  }
  else
  {
    updateState(PressurePumpStates::BYPASSCLOSE);
  }
}

void PressurePump::startBlowOff()
{
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
  Parent->getSoundObject()->playOnSound();
  if (KeepOpen)
    updateState(PressurePumpStates::BYPASSOPEN);
  else
    updateState(PressurePumpStates::BLOWOFF);
}

void PressurePump::turnBypassOff()
{
  Parent->getSoundObject()->playOffSound();
  updateState(PressurePumpStates::BYPASSCLOSE);
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
    Parent->getSoundObject()->playOnSound();
  }
}