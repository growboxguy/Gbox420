#pragma once
#include "420Common.h"
#include "420Module.h"
#include "Sound.h"
#include "Switch.h"
#include "RBDdimmer.h"   //< For the AC PWM controller
#include "movingAvg.h"   ///< Moving average calculation for Speed adjuster 10kOhm Potentiometer

class ACMotor : virtual public Common
{
public:
  ACMotor(const __FlashStringHelper *Name, Module *Parent, Settings::ACMotorSettings *DefaultSettings);
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
  void updateSpeed();  ///< Read 10kOhm potentiometer
  void setSpeed(uint8_t Speed);
  uint8_t getSpeed();
  char *getSpeedText(bool FriendlyFormat = false);
  float getRPM();
  char *getRPMText(bool FriendlyFormat = false);

private:
  uint8_t *OnOffPin = NULL;
  uint16_t *SpinOffTime = NULL;
  uint8_t *BrushPin = NULL;
  uint8_t *Coil1Pin = NULL;
  uint8_t *Coil2Pin = NULL;
  uint32_t StateTimer = millis(); ///< Used to measure time spent in a state
  float RPM = 0;
  bool StopRequested = false;     ///< Signals to stop the motor
  bool ForwardRequested = false;  ///< Signals to start the motor in forward direction
  bool BackwardRequested = false; ///< Signals to start the motor in backward direction

protected:
  Module *Parent = NULL;
  Switch *OnOffSwitch;
  Switch *BrushSwitch;
  Switch *Coil1Switch;
  Switch *Coil2Switch;
  dimmerLamp *PWMController;   ///< Provided by the RBDdimmer library  
  uint8_t *SpeedPotPin;
  movingAvg *AverageSpeedReading;
  uint8_t *SpeedLimitLow;
  uint8_t *SpeedLimitHigh;
  uint8_t *ForwardPin = NULL;  
  bool ForwardButtonPressed = false;
  uint8_t *BackwardPin = NULL;
  bool BackwardButtonPressed = false;
  ACMotorStates State = ACMotorStates::IDLE;
  uint8_t Speed = 0; ///< Motor speed (0% - 100%)
};