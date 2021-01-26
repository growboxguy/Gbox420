#include "WaterPump.h"

WaterPump::WaterPump(const __FlashStringHelper *Name, Module *Parent, Settings::WaterPumpSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  logToSerials(F(""), true, 0);  //New line
  logToSerials(F(""), false, 1); //Extra indentation
  PumpSwitch = new Switch_PWM(F("SpraySolenoid"), DefaultSettings->PumpPin, &DefaultSettings->Speed, &DefaultSettings->SpeedLowLimit, DefaultSettings->PumpPinNegativeLogic);
  PumpTimeOut = &DefaultSettings->PumpTimeOut;
  PumpEnabled = &DefaultSettings->PumpEnabled;
  if (*PumpEnabled)
  {
    State = WaterPumpStates::IDLE;
  }
  else
  {
    State = WaterPumpStates::DISABLED;
  }
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  logToSerials(F("WaterPump object created"), true, 3);
}

void WaterPump::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("State:"));
  strcat(LongMessage, getStateText());
  strcat_P(LongMessage, (PGM_P)F(" ; TimeOut:"));
  strcat(LongMessage, toText_second(*PumpTimeOut));
  logToSerials(&LongMessage, true, 1);
}

void WaterPump::refresh_Sec()
{
  if (*Debug)
  {
    Common::refresh_Sec();
  }
  updateState(State);
}

void WaterPump::updateState(WaterPumpStates NewState) ///< Without a parameter actualize the current State. When NewState parameter is passed it overwrites State
{
  if (State != NewState) ///< if not the default value was passed
  {
    State = NewState;
    PumpTimer = millis(); ///< Start measuring the time spent in the new State
  }

  switch (State)
  {  
  case WaterPumpStates::RUNNING:
    PumpSwitch->turnOn();
    *PumpEnabled = true;
    if (RunTime > 0 && millis() - PumpTimer > ((uint32_t)RunTime * 1000))  //< Check if it is time to stop
    {
      RunTime = 0;
      logToSerials(F("Running complete, stopping"), true, 3);
      updateState(WaterPumpStates::IDLE);
    }
    if (millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000)) ///< Safety feature, During normal operation this should never be reached. The caller that turned on the pump should stop it before timeout is reached
    {
      Parent->addToLog(F("ALERT: Pump timeout reached"), 3); ///< \todo send email alert
      updateState(WaterPumpStates::DISABLED);
    }
    break;  
  case WaterPumpStates::IDLE:
    PumpSwitch->turnOff();
    *PumpEnabled = true;
    break;  
  case WaterPumpStates::DISABLED:
    PumpSwitch->turnOff();
    *PumpEnabled = false;
    break;
  }
}

void WaterPump::startPump(bool ResetStatus)
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
    updateState(WaterPumpStates::RUNNING);
  }
  else
    logToSerials(F("disabled, cannot turn ON"), true, 1);
}

void WaterPump::stopPump()
{
  logToSerials(Name, false, 3);
  logToSerials(F("OFF"), true, 1);
  Parent->getSoundObject()->playOffSound();
  updateState(WaterPumpStates::IDLE);
}

void WaterPump::disablePump()
{
  logToSerials(Name, false, 3);
  logToSerials(F("disabled"), true, 1);
  Parent->getSoundObject()->playOffSound();
  updateState(WaterPumpStates::DISABLED);
}

void WaterPump::setSpeed(uint8_t DutyCycle) //Set PWM duty cycle
{
  PumpSwitch->setDutyCycle(DutyCycle);
}

WaterPumpStates WaterPump::getState()
{
  return State;
}

char *WaterPump::getStateText()
{
  return toText_waterPumpState(State);
}

bool WaterPump::getEnabledState()
{
  return *PumpEnabled;
}

int WaterPump::getPumpTimeOut()
{
  return *PumpTimeOut;
}

void WaterPump::setPumpTimeOut(int TimeOut)
{
  if (*this->PumpTimeOut != (uint16_t)TimeOut && TimeOut > 0)
  {
    *this->PumpTimeOut = TimeOut;
    logToSerials(Name, false, 1);
    logToSerials(F("timeout updated"), true, 1);
    Parent->getSoundObject()->playOnSound();
  }
}