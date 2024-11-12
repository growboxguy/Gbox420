#include "ACMotor.h"

volatile long ACMotor::TachoPulseCounter = 0;

ACMotor::ACMotor(const __FlashStringHelper *Name, Module *Parent, Settings::ACMotorSettings *MotorSettings, Settings::RelaySettings *RelaySettings) : Common(Name), Parent(Parent)
{
  State = ACMotorStates::IDLE;
  OnOffSwitch = new Switch(F("OnOff"), RelaySettings->OnOffRelayPin, &RelaySettings->NegativeLogic, RelaySettings->FlipDelay);
  BrushSwitch = new Switch(F("Brush"), RelaySettings->BrushRelayPin, &RelaySettings->NegativeLogic, RelaySettings->FlipDelay);
  Coil1Switch = new Switch(F("Coil1"), RelaySettings->Coil1RelayPin, &RelaySettings->NegativeLogic, RelaySettings->FlipDelay);
  Coil2Switch = new Switch(F("Coil2"), RelaySettings->Coil2RelayPin, &RelaySettings->NegativeLogic, RelaySettings->FlipDelay);
  ZeroCrossingPin = &MotorSettings->ZeroCrossingPin;
  ComparatorPin = &MotorSettings->ComparatorPin;
  BackwardPin = &MotorSettings->BackwardPin;
  ForwardPin = &MotorSettings->ForwardPin;
  TriacPin = &MotorSettings->TriacPin;
  TargetRPMPin = &MotorSettings->TargetRPMPin;
  PIDEnabled = &MotorSettings->PIDEnabled;
  Kp = &MotorSettings->Kp;
  Ki = &MotorSettings->Ki;
  Kd = &MotorSettings->Kd;
  TachoPulsesPerRevolution = &MotorSettings->TachoPulsesPerRevolution;
  RPMLimitMin = &MotorSettings->RPMLimitMin;
  RPMLimitMax = &MotorSettings->RPMLimitMax;
  Prescale = &MotorSettings->Prescale;
  TriacActiveTicks = &MotorSettings->TriacActiveTicks;
  TriacDelayMin = &MotorSettings->TriacDelayMin;
  TriacDelayMax = &MotorSettings->TriacDelayMax;
  SpinOffTime = &MotorSettings->SpinOffTime;
  DebounceDelay = &MotorSettings->DebounceDelay;
  pinMode(*ForwardPin, INPUT_PULLUP);
  pinMode(*BackwardPin, INPUT_PULLUP);
  pinMode(*TargetRPMPin, INPUT);
  pinMode(*ComparatorPin, INPUT_PULLUP);
  pinMode(*TriacPin, OUTPUT);
  digitalWrite(*TriacPin, LOW); ///< Turns TRIAC OFF
  PID_TriacDelay = *TriacDelayMax;
  PidController = new PID(&PID_CurrentRPM, &PID_TriacDelay, &PID_TargetRPM, *Kp, *Ki, *Kd, P_ON_E, REVERSE);
  PidController->SetOutputLimits(*TriacDelayMin, *TriacDelayMax);
  TargetRPM = new movingAvg(MovingAverageDepth);
  TargetRPM->begin();
  TargetRPM->reading(*RPMLimitMin);
  updateTargetRPM();
  TachoPulseCounter = 0;
  //INTEGRATED Comparator DISABLED//analogComparator.setOn(INTERNAL_REFERENCE, AIN1);       // AIN0: Internal 1.1V reference voltage , AIN1: external signal connected to D7
  //INTEGRATED Comparator DISABLED//analogComparator.enableInterrupt(tachoTrigger, RISING); // Call the trigger function when an intertupt is raised by the comparator (When ANT1 (D7) goes over AIN0)
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
  strcat_P(LongMessage, (PGM_P)F("\",\"R\":\""));
  strcat(LongMessage, getRPMText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"RT\":\""));
  strcat(LongMessage, getPID_TargetRPMText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, toText_milisecond(PID_TriacDelay));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void ACMotor::processTimeCriticalStuff() ///< Called every 0.1sec
{
  if (State != ACMotorStates::IDLE)
  {
    calculateRPM();
  }
}

void ACMotor::refresh_Sec()
{
  Common::refresh_Sec();
  updateTargetRPM();

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
    MotorState = false;
    OnOffSwitch->turnOff_Wait();
    BrushSwitch->turnOff();
    Coil1Switch->turnOff();
    Coil2Switch->turnOff_Wait();
    break;
  case ACMotorStates::FORWARD:
    if (getState() == ACMotorStates::IDLE)
    {
      Parent->getSoundObject()->playOnSound();
      BrushSwitch->turnOff();
      Coil1Switch->turnOff();
      Coil2Switch->turnOff_Wait();
      OnOffSwitch->turnOn_Wait();
      TachoPulseCounter = 0;            ///Reset RPM counter
      PID_CurrentRPM = 0;               ///Reset RPM
      LastRPMCalculation = millis();    ///Reset RPM timer
      PID_TriacDelay = *TriacDelayMax;   ///Reset triac delay
      PidController->SetMode(AUTOMATIC); ///Turn the PID on
      MotorState = true;
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
      BrushSwitch->turnOn();
      Coil1Switch->turnOn();
      Coil2Switch->turnOn_Wait();
      OnOffSwitch->turnOn_Wait();
      TachoPulseCounter = 0;            ///Reset RPM counter
      PID_CurrentRPM = 0;               ///Reset RPM
      LastRPMCalculation = millis();    ///Reset RPM timer
      PID_TriacDelay = *TriacDelayMax;   ///Reset triac delay
      PidController->SetMode(AUTOMATIC); ///Turn the PID on
      MotorState = true;
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
    MotorState = false;
    PidController->SetMode(MANUAL); //Turn the PID off
    delay(20); //Wait a full AC signal
    OnOffSwitch->turnOff_Wait();
    BrushSwitch->turnOff();
    Coil1Switch->turnOff();
    Coil2Switch->turnOff_Wait();    
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

void ACMotor::updateTargetRPM()
{
  TargetRPM->reading(map(analogRead(*TargetRPMPin), 0, 1023, *RPMLimitMin, *RPMLimitMax)); //take a reading and map it between the RPM
  PID_TargetRPM = TargetRPM->getAvg();
}

void ACMotor::calculateRPM()
{
  uint32_t ElapsedTime = millis() - LastRPMCalculation;
  PID_CurrentRPM = (double)TachoPulseCounter / ElapsedTime / *TachoPulsesPerRevolution * 60000;
  TachoPulseCounter = 0;
  LastRPMCalculation = millis();
}

double ACMotor::getRPM()
{
  return PID_CurrentRPM;
}

char *ACMotor::getRPMText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_rpm(PID_CurrentRPM);
  else
    return toText(PID_CurrentRPM);
}

char *ACMotor::getPID_TargetRPMText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_rpm(PID_TargetRPM);
  else
    return toText(PID_TargetRPM);
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