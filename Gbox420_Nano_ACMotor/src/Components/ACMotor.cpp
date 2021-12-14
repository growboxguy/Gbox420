#include "ACMotor.h"

volatile long ACMotor::TachoPulseCounter = 0;

ACMotor::ACMotor(const __FlashStringHelper *Name, Module *Parent, Settings::ACMotorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  State = ACMotorStates::IDLE;
  OnOffSwitch = new Switch(F("OnOff"), DefaultSettings->OnOffRelayPin, &DefaultSettings->RelayNegativeLogic);
  BrushSwitch = new Switch(F("Brush"), DefaultSettings->BrushRelayPin, &DefaultSettings->RelayNegativeLogic);
  Coil1Switch = new Switch(F("Coil1"), DefaultSettings->Coil1RelayPin, &DefaultSettings->RelayNegativeLogic);
  Coil2Switch = new Switch(F("Coil2"), DefaultSettings->Coil2RelayPin, &DefaultSettings->RelayNegativeLogic);
  TargetRPMPin = &DefaultSettings->TargetRPMPin;
  ZeroCrossingPin = &DefaultSettings->ZeroCrossingPin;
  ComparatorPin = &DefaultSettings->ComparatorPin;
  ForwardPin = &DefaultSettings->ForwardPin;
  BackwardPin = &DefaultSettings->BackwardPin;
  TriacPin = &DefaultSettings->TriacPin;
  TriacDelayMin = &DefaultSettings->TriacDelayMin;
  TriacDelayMax = &DefaultSettings->TriacDelayMax;
  TriacGateCloseDelay = &DefaultSettings->TriacGateCloseDelay;
  TachoPulsesPerRevolution = &DefaultSettings->TachoPulsesPerRevolution;
  RPMLimitMin = &DefaultSettings->RPMLimitMin;
  RPMLimitMax = &DefaultSettings->RPMLimitMax;
  Kp = &DefaultSettings->Kp;
  Ki = &DefaultSettings->Ki;
  Kd = &DefaultSettings->Kd;  
  SpinOffTime = &DefaultSettings->SpinOffTime;
  DebounceDelay = &DefaultSettings->DebounceDelay;
  pinMode(*ForwardPin, INPUT_PULLUP);
  pinMode(*BackwardPin, INPUT_PULLUP);
  pinMode(*TargetRPMPin, INPUT);
  pinMode(*ComparatorPin, INPUT_PULLUP);
  pinMode(*TriacPin, OUTPUT);
  digitalWrite(*TriacPin, LOW);       ///< Turns TRIAC OFF
  //attachInterrupt(digitalPinToInterrupt(*ComparatorPin), tachoTrigger, RISING);
  PID_Output = *TriacDelayMin;
  PidController = new PID(&PID_CurrentRPM, &PID_Output , &PID_TargetRPM, *Kp, *Ki, *Kd, P_ON_M, DIRECT);
  PidController->SetOutputLimits(*TriacDelayMin, *TriacDelayMax);
  PidController->SetMode(AUTOMATIC); //turn the PID on
  TargetRPM = new movingAvg(MovingAverageDepth);
  TargetRPM->begin();
  TargetRPM->reading(*RPMLimitMin);
  updateTargetRPM();
  TachoPulseCounter = 0;
  //INTEGRATED Comparator DISABLED//analogComparator.setOn(INTERNAL_REFERENCE, AIN1);       // AIN0: Internal 1.1V reference voltage , AIN1: external signal connected to D7
  //INTEGRATED Comparator DISABLED//analogComparator.enableInterrupt(tachoTrigger, RISING); // Call the trigger function when an intertupt is raised by the comparator (When ANT1 (D7) goes over AIN0)
 
  // set up Timer1
  OCR1A = 100;   // initialize the comparator
  TIMSK1 = 0x03; // enable comparator A and overflow interrupts
  TCCR1A = 0x00; // timer control registers set for normal operation, timer disabled
  TCCR1B = 0x00; // timer control registers set for normal operation, timer disabled
  attachInterrupt(digitalPinToInterrupt(*ZeroCrossingPin), zeroCrossingInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(*ComparatorPin), tachoTrigger, RISING); 
 
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
    //ACDimmer->setState(OFF);
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
      //ACDimmer->setState(ON);
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
      //ACDimmer->setState(ON);
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
    //ACDimmer->setState(OFF);
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

void ACMotor::updateTargetRPM()
{
  TargetRPM->reading(map(analogRead(*TargetRPMPin), 0, 1023, *RPMLimitMin, *RPMLimitMax)); //take a reading and map it between 0 - 100%
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

// Interrupt Service Routines
static void ACMotor::zeroCrossingInterrupt()
{                    //AC signal crossed zero: start the delay before turning on the TRIAC
  TCCR1B = *Prescale; // prescale the
  TCNT1 = 0;         // reset timer - count from zero
  OCR1A = Delay;     // set the compare register: triggers TIMER1_COMPA_vect when the tick counter reaches the delay calculated by the PID controller
}

static ACMotor::ISR(TIMER1_COMPA_vect)
{                 // comparator match: TRIAC delay reached after a zero crossing
  if (MotorState) ///< If the motor should be running
  {
    digitalWrite(*TriacPin, HIGH);        // turn on TRIAC gate
    TCNT1 = 65536 - *TriacGateCloseDelay; // trigger pulse width
  }
}

static ACMotor::ISR(TIMER1_OVF_vect)
{                              // timer1 overflow
  digitalWrite(*TriacPin, LOW); // turn off TRIAC gate
  TCCR1B = 0x00;               // disable timer stops unintended triggers
}

void ACMotor::tachoTrigger()
{
  TachoPulseCounter++;
}