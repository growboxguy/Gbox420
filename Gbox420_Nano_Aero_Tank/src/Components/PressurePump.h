#pragma once

#include "420Common.h"
#include "420Module.h"
#include "Sound.h"
#include "Switch_PWM.h"

///< Diaphragm pump controller

class PressurePump : virtual public Common
{
public:
  PressurePump(const __FlashStringHelper *Name, Module *Parent, Settings::PressurePumpSettings *DefaultSettings);
  void report();
  void refresh_Sec();

  void updateState(PressurePumpStates NewState = (PressurePumpStates)-1);
  void setMaxPressure(float MaxPressure);
  void setSpeed(uint8_t DutyCycle); //Set PWM duty cycle

  void startPump(bool ResetStatus = false); ///< Turn the pump ON
  void stopPump();                          ///< Turn the pump OFF
  void disablePump();                       ///< Disable the pump
  void startMixing();

  void turnBypassOn(bool KeepOpen = false);  ///< KeepOpen False - Opens the bypass vale for a short period to blow off pressure, KeepOpen True: Keep the bypass open up to BypassSolenoidMaxOpenTime
  void turnBypassOff();

  void startPrime();
  void startRunning();
  void startBlowOff();

  PressurePumpStates getState();
  char *getStateText();   ///< Text representation of the current state: DISABLED/IDLE/RUNNING...
  bool getEnabledState(); ///< Pump ENABLED(1)/DISABLED(0)

  //void checkRelay(); ///< Flit the relay into the correct status ON/OFF based on PumpOn variable

  void setPumpTimeOut(int NewTime);
  int getPumpTimeOut();
  void setPrimingTime(int NewTime);
  int getPrimingTime();
  //void setBlowOffTime(int NewTime);
  //int getBlowOffTime();
  uint32_t PumpTimer = millis(); ///< Spray cycle timer, used for State runtime tracking, - https://www.arduino.cc/reference/en/language/functions/time/millis/

private:
  PressurePumpStates State = PressurePumpStates::IDLE;

protected:
  Module *Parent;
  Switch_PWM *PumpSwitch;
  Switch *BypassSwitch;
  bool *PumpEnabled;                           ///< Enable/disable pump. false= Block running the pump
  uint8_t *Speed = NULL;                       ///< Set PWM speed of the motor (!!!Needs a MOSFET based relay!!!)
  uint16_t *PumpTimeOut = NULL;                ///< Max pump run time in seconds
  uint16_t *BypassSolenoidMaxOpenTime = NULL;  ///< (sec) Max time the bypass can stay open
  uint16_t *BypassSolenoidClosingDelay = NULL; ///< (ms) How long it takes for the solenoid to close
  int *PrimingTime = NULL;                     ///< (Sec) For how long to keep the bypass solenoid on when starting the pump - Remove air bubbles from pump intake side
  int *BlowOffTime = NULL;                     ///< (Sec) For how long to open the bypass solenoid on after turning the pump off - Release pressure from pump discharge side
};