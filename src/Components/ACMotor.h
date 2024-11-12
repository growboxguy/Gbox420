#pragma once

#include "420Common.h"
#include "420Module.h"
#include "Sound.h"
#include "Switch.h"
#include "Switch_PWM.h"

class ACMotor : virtual public Common
{
public:
  ACMotor(const __FlashStringHelper *Name, Module *Parent, Settings::ACMotorSettings *DefaultSettings);
  void refresh_Sec();
  void report(bool FriendlyFormat = false);
  void updateState(ACMotorStates NewState);
  ACMotorStates getState();
  char *getStateText(bool FriendlyFormat = false); ///< Returns the state of the AC Motor
  void stop();        ///< Stop the AC Motor  - Takes time to apply
  void stopRequest(); ///< Signals to stop the AC Motor at the next refresh - Runs fast
  void forward();
  void forwardRequest();
  void backward();
  void backwardRequest();
  void setSpeed(uint8_t Speed);  
  uint8_t getSpeed(); 
  char *getSpeedText(bool FriendlyFormat = false);
  float getRPM();
  char *getRPMText(bool FriendlyFormat = false);

private:
  uint8_t OnOffPin;      ///< Pin for the on/off switch
  uint16_t SpinOffTime;  ///< Time to spin off the motor
  uint8_t BrushPin;      ///< Pin for the brush relay
  uint8_t Coil1Pin;      ///< Pin for coil 1
  uint8_t Coil2Pin;      ///< Pin for coil 2
  uint32_t StateTimer;   ///< Timer to measure time spent in a state
  float RPM;             ///< RPM of the motor
  bool StopRequested;    ///< Signals to stop the motor
  bool ForwardRequested; ///< Signals to start the motor in forward direction
  bool BackwardRequested; ///< Signals to start the motor in backward direction

protected:
  Module *Parent;
  Switch OnOffSwitch;    ///< On/off switch object
  Switch BrushSwitch;    ///< Brush relay switch object
  Switch Coil1Switch;    ///< Coil 1 relay switch object
  Switch Coil2Switch;    ///< Coil 2 relay switch object
  Switch_PWM SpeedSwitchPWM; ///< PWM speed control for the motor
  ACMotorStates State;   ///< Current state of the motor  
  uint8_t Speed;         ///< Motor speed (0% - 100%)
};
