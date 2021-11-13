#pragma once
#include "420Common.h"
#include "420Module.h"
#include "Sound.h"

class ACMotor : virtual public Common
{
public:
  ACMotor(const __FlashStringHelper *Name, Module *Parent, Settings::ACMotorSettings *DefaultSettings);
  void refresh_Sec();
  void refresh_FiveSec();
  void report(bool FriendlyFormat = false);
  void updateState(ACMotorStates NewState);
  ACMotorStates getState();
  char *getStateText(bool FriendlyFormat = false); ///< Retuns the numerical or text state of the AC Motor
  void stop();        ///< Stop the AC Motor  - Takes time to apply
  void stopRequest(); ///< Signals to Stop the AC Motor at the next refresh - Runs fast
  void forward();
  void forwardRequest();
  void backward();
  void backwardRequest();
  void setSpeed(uint8_t Speed);
  char *getSpeedText(bool FriendlyFormat = false);

private:
  uint32_t OnTimer = millis();     ///< Used to measure the ON time 
  uint8_t Speed = 50;
  float RPM = 0;
  bool StopRequested = false;       ///< Signals to stop the motor
  bool ForwardRequested = false; ///< Signals to start the motor in forward direction
  bool BackwardRequested = false;  ///< Signals to start the motor in backward direction

protected:
  Module *Parent;
  ACMotorStates State = ACMotorStates::IDLE;
};