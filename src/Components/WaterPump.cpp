#include "WaterPump.h"

WaterPump::WaterPump(const __FlashStringHelper *Name, Module *Parent, Settings::WaterPumpSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  logToSerials(F(""), true, 0);  //New line
  logToSerials(F(""), false, 1); //Extra indentation
  PumpSwitch = new Switch_PWM(F("PumpPWM"), DefaultSettings->PumpPin, &DefaultSettings->Speed, &DefaultSettings->SpeedLowLimit, DefaultSettings->PumpPinNegativeLogic);
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
  logToSerials(F("WaterPump ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void WaterPump::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, getStateText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"T\":\""));
  strcat(LongMessage, getTimeOutText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void WaterPump::refresh_Sec()
{
  Common::refresh_Sec();
  updateState(State);

  if (StartPumpRequested)
  {
    StartPumpRequested = false;
    startPump(true);
    Parent->addToLog(getName(F("started")));
  }
  if (StopPumpRequested)
  {
    StopPumpRequested = false;
    stopPump(true);
    Parent->addToLog(getName(F("stopped")));
  }
  if (DisablePumpRequested)
  {
    DisablePumpRequested = false;
    disablePump();
    Parent->addToLog(getName(F("disabled")));
  }
}

void WaterPump::updateState(WaterPumpStates NewState) ///< When NewState parameter is passed it overwrites State
{
  if (State != NewState)
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); //reset variable to store the Publish to path
    strcpy_P(LongMessage, (PGM_P)Name);
    strcat_P(LongMessage, (PGM_P)F(" state: "));
    strcat(LongMessage, toText_waterPumpState(State));
    strcat_P(LongMessage, (PGM_P)F(" -> "));
    strcat(LongMessage, toText_waterPumpState(NewState));
    logToSerials(&LongMessage, true, 3);

    State = NewState;
    StateTimer = millis(); ///< Start measuring the time spent in the new State
  }

  switch (NewState)
  {
  case WaterPumpStates::DISABLED:
    PumpSwitch->turnOff();
    *PumpEnabled = false;
    break;
  case WaterPumpStates::IDLE:
    PumpSwitch->turnOff();
    *PumpEnabled = true;
    break;
  case WaterPumpStates::RUNNING:
    PumpSwitch->turnOn();
    *PumpEnabled = true;
    if (RunTime > 0 && millis() - StateTimer > ((uint32_t)RunTime * 1000)) //< Check if it is time to stop
    {
      RunTime = 0;
      logToSerials(getName(F("finished")), true, 3);
      updateState(WaterPumpStates::IDLE);
    }
    if (millis() - StateTimer > ((uint32_t)*PumpTimeOut * 1000)) ///< Safety feature, During normal operation this should never be reached. The caller that turned on the pump should stop it before timeout is reached
    {
      Parent->addToLog(getName(F("timeout")));
      updateState(WaterPumpStates::DISABLED);
    }
    break;
  }
}

void WaterPump::startPumpRequest() //Stores the request only, will apply the next time the Pump is refreshing
{
  StartPumpRequested = true;
}

void WaterPump::startPump(bool ResetState)
{
  Parent->getSoundObject()->playOnSound();
  if (*PumpEnabled || ResetState)
  {
    updateState(WaterPumpStates::RUNNING);
  }
  else
  {
    logToSerials(getName(F("timeout")), true, 1);
  }
}

void WaterPump::stopPumpRequest() //Stores the request only, will apply the next time the Pump is refreshing
{
  StopPumpRequested = true;
}

void WaterPump::stopPump(bool ResetState)
{
  if ((State != WaterPumpStates::IDLE && State != WaterPumpStates::DISABLED) || (State == WaterPumpStates::DISABLED && ResetState))
  {
    Parent->getSoundObject()->playOffSound();
    if (*PumpEnabled || ResetState)
    {
      updateState(WaterPumpStates::IDLE);
    }
    else
    {
      updateState(WaterPumpStates::DISABLED);
    }
  }
}

void WaterPump::disablePumpRequest() //Stores the request only, will apply the next time the Pump is refreshing
{
  DisablePumpRequested = true;
}

void WaterPump::disablePump()
{
  if (State != WaterPumpStates::DISABLED)
  {
    Parent->getSoundObject()->playOffSound();
    updateState(WaterPumpStates::DISABLED);
  }
}

void WaterPump::setSpeed(uint8_t DutyCycle) //Set PWM duty cycle
{
  PumpSwitch->setDutyCycle(DutyCycle);
}

uint8_t WaterPump::getSpeed() //Get PWM duty cycle
{
  return PumpSwitch->getDutyCycle();
}

char *WaterPump::getSpeedText(bool FriendlyFormat) //Get PWM duty cycle text
{
  return PumpSwitch->getDutyCycleText(FriendlyFormat);
}

WaterPumpStates WaterPump::getState()
{
  return State;
}

char *WaterPump::getStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_waterPumpState(State);
  }
  else
  {
    return toText((int)State);
  }
}

bool WaterPump::getEnabledState()
{
  return *PumpEnabled;
}

void WaterPump::setTimeOut(uint16_t TimeOut)
{
  if (*this->PumpTimeOut != TimeOut && TimeOut > 0)
  {
    *this->PumpTimeOut = TimeOut;
    logToSerials(getName(F("timeout updated")), true, 1);
    Parent->getSoundObject()->playOnSound();
  }
}

int WaterPump::getTimeOut()
{
  return *PumpTimeOut;
}

char *WaterPump::getTimeOutText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_second(*PumpTimeOut);
  else
    return toText(*PumpTimeOut);
}