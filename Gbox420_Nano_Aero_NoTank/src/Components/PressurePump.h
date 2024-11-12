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
  void report(bool FriendlyFormat = false);
  void refresh_Sec();
  void updateState(PressurePumpStates NewState = (PressurePumpStates)-1);
  void setMaxPressure(float MaxPressure);
  void setSpeed(uint8_t DutyCycle);        // Set PWM duty cycle
  void startPump(bool ResetState = false); ///< Turn the pump ON
  void stopPump(bool ResetState = false);  ///< Turn the pump OFF
  void disablePump();                      ///< Disable the pump
  void startMixing();
  void turnBypassOn(bool KeepOpen = false); ///< KeepOpen False - Opens the bypass valve for a short period to blow off pressure, KeepOpen True: Keep the bypass open up to BypassSolenoidMaxOpenTime
  void turnBypassOff();
  void startPrime();
  void startRunning();
  void startBlowOff();
  PressurePumpStates getState();
  char *getStateText(bool FriendlyFormat = false); ///< Text representation of the current state: DISABLED/IDLE/RUNNING...
  bool getEnabledState();                          ///< Pump ENABLED(1)/DISABLED(0)
  void setTimeOut(int NewTime);
  int getTimeOut();
  char *getTimeOutText(bool FriendlyFormat = false);
  void setPrimingTime(int NewTime);
  int getPrimingTime();
  char *getPrimingTimeText(bool FriendlyFormat = false);
  uint32_t PumpTimer = millis(); ///< Spray cycle timer, used for State runtime tracking, - https://www.arduino.cc/reference/en/language/functions/time/millis/

private:
  PressurePumpStates State = PressurePumpStates::IDLE;

protected:
  Module *Parent;
  Switch_PWM *PumpSwitch;
  Switch *BypassSwitch;
  bool &PumpEnabled;                    ///< Enable/disable pump. false= Block running the pump
  uint8_t &Speed;                       ///< Set PWM speed of the motor (!!!Needs a MOSFET based relay!!!)
  uint16_t &PumpTimeOut;                ///< Max pump run time in seconds
  uint16_t &BypassSolenoidMaxOpenTime;  ///< (sec) Max time the bypass can stay open
  uint16_t &BypassSolenoidClosingDelay; ///< (ms) How long it takes for the solenoid to close
  uint16_t &PrimingTime;                ///< (Sec) For how long to keep the bypass solenoid on when starting the pump - Remove air bubbles from pump intake side
  uint16_t &BlowOffTime;                ///< (Sec) For how long to open the bypass solenoid on after turning the pump off - Release pressure from pump discharge side
};