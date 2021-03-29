#pragma once

#include "420Common.h"
#include "420Module.h"
#include "Sound.h"
#include "Switch_PWM.h"

///< Diaphragm pump controller
class WaterPump : virtual public Common
{
public:
  WaterPump(const __FlashStringHelper *Name, Module *Parent, Settings::WaterPumpSettings *DefaultSettings);
  void report(bool FriendlyFormat = false);
  void refresh_Sec();
  void updateState(WaterPumpStates NewState = (WaterPumpStates)-1);
  void setSpeed(uint8_t DutyCycle);        //Set Motor speed - Adjust PWM duty cycle
  void startPump(bool ResetState = false); ///< Turn the pump ON
  void stopPump(bool ResetState = false);  ///< Turn the pump OFF
  void disablePump();                      ///< Disable the pump

  WaterPumpStates getState(); ///< Get the current pump state
  char *getStateText(bool FriendlyFormat = false);   ///< Text representation of the current state: DISABLED/IDLE/RUNNING..
  bool getEnabledState();     ///< Pump ENABLED(1)/DISABLED(0)
  void setPumpTimeOut(uint16_t NewTime);
  int getPumpTimeOut();
  char *getTimeOutText(bool FriendlyFormat = false);

private:
  WaterPumpStates State = WaterPumpStates::IDLE;
  uint32_t StateTimer = millis(); ///< Spray cycle timer, used for State runtime tracking, - https://www.arduino.cc/reference/en/language/functions/time/millis/

protected:
  Module *Parent;
  Switch_PWM *PumpSwitch;
  bool *PumpEnabled;            ///< Enable/disable pump. false= Block running the pump
  uint8_t *Speed = NULL;        ///< Set PWM speed of the motor (!!!Needs a MOSFET based relay!!!)
  int RunTime = 0;              ///< Max pump run time in seconds
  uint16_t *PumpTimeOut = NULL; ///< Max pump run time in seconds
};