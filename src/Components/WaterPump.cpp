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

void WaterPump::report(bool JSONReport)
{
  Common::report(JSONReport); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
     ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
    strcat(LongMessage, getStateText());
    strcat_P(LongMessage, (PGM_P)F("\",\"TO\":\""));
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

void WaterPump::refresh_Sec()
{
  if (*Debug)
  {
    Common::refresh_Sec();
  }
  updateState(State);
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
      appendName(true);
      strcat_P(ShortMessage, (PGM_P)F("finished"));
      logToSerials(&ShortMessage, true, 3);
      updateState(WaterPumpStates::IDLE);
    }
    if (millis() - StateTimer > ((uint32_t)*PumpTimeOut * 1000)) ///< Safety feature, During normal operation this should never be reached. The caller that turned on the pump should stop it before timeout is reached
    {
      appendName(true);
      strcat_P(ShortMessage, (PGM_P)F("timeout"));
      Parent->addToLog(ShortMessage);
      updateState(WaterPumpStates::DISABLED);
    }
    break;
  }
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
    appendName(true);
    strcat_P(ShortMessage, (PGM_P)F("timeout"));
    logToSerials(&ShortMessage, true, 1);
  }
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
    appendName(true);
    strcat_P(ShortMessage, (PGM_P)F("timeout updated"));
    logToSerials(&ShortMessage, true, 1);
    Parent->getSoundObject()->playOnSound();
  }
}