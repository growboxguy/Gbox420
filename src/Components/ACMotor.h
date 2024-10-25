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
  char *getStateText(bool FriendlyFormat = false); ///< Retuns the state of the AC Motor
  void stop();        ///< Stop the AC Motor  - Takes time to apply
  void stopRequest(); ///< Signals to Stop the AC Motor at the next refresh - Runs fast
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
  uint8_t *OnOffPin = NULL;
  uint16_t *SpinOffTime = NULL;
  uint8_t *BrushPin = NULL;
  uint8_t *Coil1Pin = NULL;
  uint8_t *Coil2Pin = NULL;;
  uint32_t StateTimer = millis();     ///< Used to measure time spent in a state
  float RPM = 0;
  bool StopRequested = false;       ///< Signals to stop the motor
  bool ForwardRequested = false; ///< Signals to start the motor in forward direction
  bool BackwardRequested = false;  ///< Signals to start the motor in backward direction

protected:
  Module *Parent = NULL;
  Switch *OnOffSwitch;
  Switch *BrushSwitch;
  Switch *Coil1Switch;
  Switch *Coil2Switch;
  Switch_PWM *SpeedSwitchPWM;
  ACMotorStates State = ACMotorStates::IDLE;  
  uint8_t *Speed = NULL;   ///< Motor speed (0% - 100%)
};