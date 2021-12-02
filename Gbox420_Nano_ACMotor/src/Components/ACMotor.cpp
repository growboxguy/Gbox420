#include "ACMotor.h"

volatile long ACMotor::TachoPulseCounter = 0;

ACMotor::ACMotor(const __FlashStringHelper *Name, Module *Parent, Settings::ACMotorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  State = ACMotorStates::IDLE;
  Speed = DefaultSettings->Speed;
  ForwardPin = &DefaultSettings->ForwardPin;
  BackwardPin = &DefaultSettings->BackwardPin;
  RPMTargetPin = &DefaultSettings->RPMTargetPin;
  RPMTargetMin = &DefaultSettings->RPMTargetMin;
  RPMTargetMax = &DefaultSettings->RPMTargetMax;
  ACDimmerLimitMin = &DefaultSettings->ACDimmerLimitMin;
  ACDimmerLimitMax = &DefaultSettings->ACDimmerLimitMax;
  Kp = &DefaultSettings->Kp;
  Ki = &DefaultSettings->Ki;
  Kd = &DefaultSettings->Kd;
  TachoPulsesPerRevolution = &DefaultSettings->TachoPulsesPerRevolution;
  SpinOffTime = &DefaultSettings->SpinOffTime;
  pinMode(*ForwardPin, INPUT_PULLUP);
  pinMode(*BackwardPin, INPUT_PULLUP);
  pinMode(*RPMTargetPin, INPUT);
  PidController = new PID(&RPMCurrent, &Speed, &RPMTarget, *Kp, *Ki, *Kd, P_ON_M, DIRECT);
  PidController->SetMode(AUTOMATIC); //turn the PID on
  AverageRPMTarget = new movingAvg(MovingAverageDepth);
  AverageRPMTarget->begin();
  analogComparator.setOn(INTERNAL_REFERENCE, AIN1);       // AIN0: Internal 1.1V reference voltage , AIN1: external signal connected to D7
  analogComparator.enableInterrupt(tachoTrigger, RISING); // Call the trigger function when an intertupt is raised by the comparator (When ANT1 (D7) goes over AIN0)
  OnOffSwitch = new Switch(F("OnOff"), DefaultSettings->OnOffPin, &DefaultSettings->RelayNegativeLogic);
  BrushSwitch = new Switch(F("Brush"), DefaultSettings->BrushPin, &DefaultSettings->RelayNegativeLogic);
  Coil1Switch = new Switch(F("Coil1"), DefaultSettings->Coil1Pin, &DefaultSettings->RelayNegativeLogic);
  Coil2Switch = new Switch(F("Coil2"), DefaultSettings->Coil2Pin, &DefaultSettings->RelayNegativeLogic);
  ACDimmer = new dimmerLamp(DefaultSettings->ACDimmerPWMPin);
  ACDimmer->begin(NORMAL_MODE, OFF); //dimmer initialisation: name.begin(Mode: NORMAL_MODE/TOGGLE_MODE, State:ON/OFF)
  ACDimmer->setPower(Speed);
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
  strcat_P(LongMessage, (PGM_P)F("\",\"RT\":\""));
  strcat(LongMessage, getRPMTargetText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void ACMotor::refresh_Sec()
{
  Common::refresh_Sec();
  readRPMTarget();
  if (State != ACMotorStates::IDLE)
  {
    updateRPM();
  }

  if (digitalRead(*ForwardPin) == LOW) ///< Currently pressed
  {
    logToSerials(F("Forward down"), true, 1);
    ForwardButtonPressed = true;
    ForwardRequested = true;
  }
  else if (ForwardButtonPressed) ///< Currently not pressed, but it was pressed the last time: turn off the motor
  {
    ForwardButtonPressed = false;
    ForwardRequested = false;
    StopRequested = true;
  }

  if (digitalRead(*BackwardPin) == LOW)
  {
    logToSerials(F("Backward down"), true, 1);
    BackwardButtonPressed = true;
    BackwardRequested = true;
  }
  else if (BackwardButtonPressed) ///< If not, but it was pressed the last time: turn off the motor
  {
    BackwardButtonPressed = false;
    BackwardRequested = false;
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
    ACDimmer->setState(OFF);
    OnOffSwitch->turnOff();
    BrushSwitch->turnOff();
    Coil1Switch->turnOff();
    Coil2Switch->turnOff();
    break;
  case ACMotorStates::FORWARD:
    if (getState() == ACMotorStates::IDLE)
    {
      Parent->getSoundObject()->playOnSound();
      OnOffSwitch->turnOn();
      BrushSwitch->turnOff();
      Coil1Switch->turnOff();
      Coil2Switch->turnOff();
      ACDimmer->setState(ON);
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
      ACDimmer->setState(ON);
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
    ACDimmer->setState(OFF);
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

void ACMotor::readRPMTarget()
{
  AverageRPMTarget->reading(map(analogRead(*RPMTargetPin), 0, 1023, *RPMTargetMin, *RPMTargetMax)); //take a reading and map it between 0 - 100%
  RPMTarget = AverageRPMTarget->getAvg();
}

void ACMotor::setSpeed(uint8_t NewSpeed)
{
  if (Speed != NewSpeed)
  {
    Speed = NewSpeed;
    logToSerials(F("Motor speed"), false, 1);
    logToSerials(Speed, true, 1);
    ACDimmer->setPower(Speed);
  }
}

uint8_t ACMotor::getSpeed()
{
  return Speed;
}

char *ACMotor::getSpeedText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_percentage(Speed);
  }
  else
  {
    return toText(Speed);
  }
}

void ACMotor::updateRPM()
{
  uint32_t ElapsedTime = millis() - RPMLastCalculation;
  RPMCurrent = (double)TachoPulseCounter / ElapsedTime / *TachoPulsesPerRevolution * 60000;
  if (PidController->Compute())
  {
    ACDimmer->setPower(map(Speed, 0, 255, *ACDimmerLimitMin, *ACDimmerLimitMax));
  }
  TachoPulseCounter = 0;
  RPMLastCalculation = millis();
}

void ACMotor::tachoTrigger()
{
  TachoPulseCounter++;
}

double ACMotor::getRPM()
{
  return RPMCurrent;
}

char *ACMotor::getRPMText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_rpm(RPMCurrent);
  else
    return toText(RPMCurrent);
}

char *ACMotor::getRPMTargetText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_rpm(RPMTarget);
  else
    return toText(RPMTarget);
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