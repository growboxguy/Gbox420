#include "ACMotor.h"

long ACMotor::TachoPulseCounter = 0;

ACMotor::ACMotor(const __FlashStringHelper *Name, Module *Parent, Settings::ACMotorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  State = ACMotorStates::IDLE;
  ForwardPin = &DefaultSettings->ForwardPin;
  pinMode(*ForwardPin, INPUT_PULLUP);
  BackwardPin = &DefaultSettings->BackwardPin;
  pinMode(*BackwardPin, INPUT_PULLUP);
  SpeedPotPin = &DefaultSettings->SpeedPotPin;
  pinMode(*SpeedPotPin, INPUT);
  AverageSpeedReading = new movingAvg(MovingAverageDepth);
  AverageSpeedReading->begin();
  Speed = DefaultSettings->Speed;
  SpeedLimitLow = &DefaultSettings->SpeedLimitLow;
  SpeedLimitHigh = &DefaultSettings->SpeedLimitHigh;
  TachoPulsesPerRevolution = &DefaultSettings->TachoPulsesPerRevolution;
  SpinOffTime = &DefaultSettings->SpinOffTime;
  analogComparator.setOn(INTERNAL_REFERENCE, AIN1);                   // AINO (D6) , the external signal is connected to ANI1 (D7)
  analogComparator.enableInterrupt(triggerRPM, RISING); // Call the trigger function when an intertupt is raised by the comperator (When ANT1 (D7) goes over AIN0)
  OnOffSwitch = new Switch(F("OnOff"), DefaultSettings->OnOffPin, &DefaultSettings->RelayNegativeLogic);
  BrushSwitch = new Switch(F("Brush"), DefaultSettings->BrushPin, &DefaultSettings->RelayNegativeLogic);
  Coil1Switch = new Switch(F("Coil1"), DefaultSettings->Coil1Pin, &DefaultSettings->RelayNegativeLogic);
  Coil2Switch = new Switch(F("Coil2"), DefaultSettings->Coil2Pin, &DefaultSettings->RelayNegativeLogic);
  PWMController = new dimmerLamp(DefaultSettings->SpeedPWMPin);
  PWMController->begin(NORMAL_MODE, OFF); //dimmer initialisation: name.begin(Mode: NORMAL_MODE/TOGGLE_MODE, State:ON/OFF)
  PWMController->setPower(Speed);
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

  updateSpeed();

  //if (State == ACMotorStates::FORWARD || State == ACMotorStates::BACKWARD)
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
    PWMController->setState(OFF);
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
      PWMController->setState(ON);
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
      PWMController->setState(ON);
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
    PWMController->setState(OFF);
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

void ACMotor::updateSpeed()
{
  AverageSpeedReading->reading(map(analogRead(*SpeedPotPin), 0, 1023, *SpeedLimitLow, *SpeedLimitHigh)); //take a reading and map it between 0 - 100%
  setSpeed(AverageSpeedReading->getAvg());
}

void ACMotor::setSpeed(uint8_t NewSpeed)
{
  if (Speed != NewSpeed)
  {
    Speed = NewSpeed;
    logToSerials(F("Motor speed"), false, 1);
    logToSerials(Speed, true, 1);
    PWMController->setPower(Speed);
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
  logToSerials(TachoPulseCounter, true, 1);
  uint32_t ElapsedTime = millis() - RPMLastCalculation;
  logToSerials(ElapsedTime, true, 1);
  RPM = (float)TachoPulseCounter / ElapsedTime / *TachoPulsesPerRevolution * 60000;
  TachoPulseCounter = 0;
  RPMLastCalculation = millis();

  logToSerials(F("Motor RPM:"), false, 1);
  logToSerials(RPM, true, 1);
}

void ACMotor::triggerRPM()
{
  TachoPulseCounter++;
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