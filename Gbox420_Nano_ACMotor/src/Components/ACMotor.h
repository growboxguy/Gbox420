#pragma once
#include "420Common.h"
#include "420Module.h"
#include "Sound.h"
#include "Switch.h"
#include "PID_v1.h"    ///< PID controller
#include "RBDdimmer.h" ///< For the AC PWM controller
//INTEGRATED COMPERATOR DISABLED// #include "analogComp.h" ///< Internal comparator library
#include "movingAvg.h" ///< Moving average calculation for Speed adjuster 10kOhm potentiometer

class ACMotor : virtual public Common
{
public:
  ACMotor(const __FlashStringHelper *Name, Module *Parent, Settings::ACMotorSettings *DefaultSettings);
  void processTimeCriticalStuff(); ///< Process things that cannot wait or need precise timing
  void refresh_Sec();
  void report(bool FriendlyFormat = false);
  void updateState(ACMotorStates NewState);
  ACMotorStates getState();
  char *getStateText(bool FriendlyFormat = false); ///< Retuns the state of the AC Motor
  void stop();                                     ///< Stop the AC Motor  - Takes time to apply
  void stopRequest();                              ///< Signals to Stop the AC Motor at the next refresh - Runs fast
  void forward();
  void forwardRequest();
  void backward();
  void backwardRequest();

  void setSpeed(uint8_t Speed);
  uint8_t getSpeed();
  char *getSpeedText(bool FriendlyFormat = false);
  void updateRPM();
  static void tachoTrigger();
  double getRPM();
  char *getRPMText(bool FriendlyFormat = false);
  void readRPMTarget(); ///< Read 10kOhm potentiometer
  char *getRPMTargetText(bool FriendlyFormat = false);

private:
  uint16_t *SpinOffTime = NULL;
  uint32_t StateTimer = millis();         ///< Used to measure time spent in a state
  static volatile long TachoPulseCounter; ///< Count the total number of interrupts
  double RPMCurrent = 0;                  ///< Calculated current RPM
  double RPMTarget = 0;                   ///< Target RPM - Adjusted by 10k potentiometer
  double *Kp = NULL;                      ///< PID tuning parameter - proportional gain
  double *Ki = NULL;                      ///< PID tuning parameter - integral gain
  double *Kd = NULL;                      ///< PID tuning parameter - derivative gain
  uint32_t RPMLastCalculation = millis(); ///< Timestamp of the last RPM calculation
  bool StopRequested = false;             ///< Signals to stop the motor
  bool ForwardRequested = false;          ///< Signals to start the motor in forward direction
  bool BackwardRequested = false;         ///< Signals to start the motor in backward direction

protected:
  Module *Parent = NULL;
  ACMotorStates State = ACMotorStates::IDLE;
  double Speed = 0; ///< Motor speed (0% - 100%)
  Switch *OnOffSwitch = NULL;
  Switch *BrushSwitch = NULL;
  Switch *Coil1Switch = NULL;
  Switch *Coil2Switch = NULL;
  dimmerLamp *ACDimmer = NULL;
  PID *PidController = NULL;
  uint8_t *ACDimmerLimitMin = NULL;
  uint8_t *ACDimmerLimitMax = NULL;
  uint8_t *TachoPulsesPerRevolution = NULL;
  uint8_t *ComparatorPin = NULL; ///< External comparator output -> triggers an interoupt (Nano supports this on D2 or D3 ports)
  uint8_t *ForwardPin = NULL;    ///< Button pin
  bool ForwardButtonPressed = false;
  uint8_t *BackwardPin = NULL; ///< Button pin
  bool BackwardButtonPressed = false;
  uint8_t *RPMTargetPin = NULL;
  movingAvg *AverageRPMTarget = NULL;
  uint16_t *RPMTargetMin = NULL;
  uint16_t *RPMTargetMax = NULL;
};