#include "ACMotor.h"

ACMotor::ACMotor(const __FlashStringHelper *Name, Module *Parent, Settings::ACMotorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  State = ACMotorStates::IDLE;
  ForwardPin = &DefaultSettings->ForwardPin;
  pinMode(*ForwardPin, INPUT_PULLUP);
  BackwardPin = &DefaultSettings->BackwardPin;
  pinMode(*BackwardPin, INPUT_PULLUP);
  Speed = &DefaultSettings->Speed;
  SpinOffTime = &DefaultSettings->SpinOffTime;
  OnOffSwitch = new Switch(F("OnOff"), DefaultSettings->OnOffPin, &DefaultSettings->RelayNegativeLogic);
  BrushSwitch = new Switch(F("Brush"), DefaultSettings->BrushPin, &DefaultSettings->RelayNegativeLogic);
  Coil1Switch = new Switch(F("Coil1"), DefaultSettings->Coil1Pin, &DefaultSettings->RelayNegativeLogic);
  Coil2Switch = new Switch(F("Coil2"), DefaultSettings->Coil2Pin, &DefaultSettings->RelayNegativeLogic);
  SpeedSwitchPWM = new Switch_PWM(F("Speed"), DefaultSettings->SpeedPWMPin, *Speed, &DefaultSettings->SpeedLimitLow, &DefaultSettings->SpeedLimitHigh);
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  logToSerials(F("ACMotor ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void ACMotor::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, getStateText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"Sp\":\""));
  strcat(LongMessage, getSpeedText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"R\":\""));
  strcat(LongMessage, getRPMText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void ACMotor::refresh_Sec()
{
  Common::refresh_Sec();

  if (digitalRead(*ForwardPin) == HIGH) ///< Is the button currently pressed?
  {
    logToSerials(F("Forward down"));
    ForwardButtonPressed = true;
    ForwardRequested = true;
  }
  else if (ForwardButtonPressed) ///< If not, but it was pressed the last time: turn off the motor
  {
    ForwardButtonPressed = false;
    StopRequested = true;
  }

  if (digitalRead(*BackwardPin) == HIGH)
  {
    logToSerials(F("Backward down"));
    BackwardButtonPressed = true;
    BackwardRequested = true;
  }
  else if (BackwardButtonPressed) ///< If not, but it was pressed the last time: turn off the motor
  {
    BackwardButtonPressed = false;
    StopRequested = true;
  }

  if (StopRequested)
  {
    StopRequested = false;
    stop();
  }
  if (ForwardRequested && State != ACMotorStates::STOPPING)
  {
    forward();
  }
  if (BackwardRequested && State != ACMotorStates::STOPPING)
  {
    backward();
  }
  if (State != ACMotorStates::IDLE)
  {
    updateState(State);
  }
}

void ACMotor::updateState(ACMotorStates NewState)
{
  bool BlockOverWritingState = false; //Used when a state transitions to a new state
  if (State != NewState)
  {
    StateTimer = millis();                         ///< Start measuring the time spent in the new State
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat(LongMessage, getName(F("state: ")));
    strcat(LongMessage, toText_ACMotorState(State));
    strcat_P(LongMessage, (PGM_P)F(" -> "));
    strcat(LongMessage, toText_ACMotorState(NewState));
    logToSerials(&LongMessage, true, 3);
  }

  switch (NewState)
  {
  case ACMotorStates::IDLE:
    digitalWrite(*OnOffPin, HIGH);
    break;
  case ACMotorStates::FORWARD:
    if (getState() == ACMotorStates::IDLE)
    {
      Parent->getSoundObject()->playOnSound();
      OnOffSwitch->turnOn();
      BrushSwitch->turnOff();
      Coil1Switch->turnOff();
      Coil2Switch->turnOff();
      ForwardRequested = false;
    }
    else if (State != NewState)
    {
      ForwardRequested = true;
      updateState(ACMotorStates::STOPPING);
      BlockOverWritingState = true;
    }
    break;
  case ACMotorStates::BACKWARD:
    if (getState() == ACMotorStates::IDLE)
    {
      Parent->getSoundObject()->playOnSound();
      OnOffSwitch->turnOn();
      BrushSwitch->turnOn();
      Coil1Switch->turnOn();
      Coil2Switch->turnOn();
      BackwardRequested = false;
    }
    else if (State != NewState)
    {
      BackwardRequested = true;
      updateState(ACMotorStates::STOPPING);
      BlockOverWritingState = true;
    }
    break;
  case ACMotorStates::STOPPING:
    OnOffSwitch->turnOff();
    BrushSwitch->turnOff();
    Coil1Switch->turnOff();
    Coil2Switch->turnOff();
    if (millis() - StateTimer > ((uint32_t)*SpinOffTime * 1000)) ///< Waiting for the motor to stop spinning
    {
      updateState(ACMotorStates::IDLE);
      BlockOverWritingState = true;
    }

    break;
  }

  if (State != NewState && !BlockOverWritingState)
  {
    State = NewState;
  }
}

void ACMotor::stop() //Takes time, do not call directly from an interupt (ESP-link website would timeout)
{
  if (State != ACMotorStates::IDLE)
    updateState(ACMotorStates::STOPPING);
  Parent->getSoundObject()->playOffSound();
}

void ACMotor::stopRequest() //Stores the request only, will apply the next time the Hempy Bucket is refreshing
{
  BackwardRequested = false;
  ForwardRequested = false;
  StopRequested = true;
}

void ACMotor::forward()
{
  updateState(ACMotorStates::FORWARD);
}

void ACMotor::forwardRequest() //Stores the request only, will apply the next time the Hempy Bucket is refreshing
{
  StopRequested = false;
  BackwardRequested = false;
  ForwardRequested = true;
}

void ACMotor::backward()
{
  updateState(ACMotorStates::BACKWARD);
}

void ACMotor::backwardRequest() //Stores the request only, will apply the next time the Hempy Bucket is refreshing
{
  ForwardRequested = false;
  StopRequested = false;
  BackwardRequested = true;
}

void ACMotor::setSpeed(uint8_t NewSpeed)
{
  if (*Speed != NewSpeed)
  {
    *Speed = NewSpeed;
    Parent->getSoundObject()->playOnSound();
  }
}

uint8_t ACMotor::getSpeed()
{
  return *Speed;
}

char *ACMotor::getSpeedText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_percentage(*Speed);
  }
  else
  {
    return toText(*Speed);
  }
}

float ACMotor::getRPM()
{
  return RPM;
}

char *ACMotor::getRPMText(bool FriendlyFormat)
{
  return toText(RPM);
}

ACMotorStates ACMotor::getState()
{
  return State;
}

char *ACMotor::getStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_ACMotorState(State);
  }
  else
  {
    return toText((int)State);
  }
}