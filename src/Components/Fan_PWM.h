/*! \file 
 *  \brief     PWM control AC power to a Fan
 *  \details   Requires a Robodyn AC Light Dimming Module with Zero-cross detection
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/
 *  \version   4.20
 */

#pragma once

#include "RBDdimmer.h"   //Library: https://github.com/RobotDynOfficial/RBDDimmer
#include "420Common.h"
#include "420Module.h"

class Fan_PWM : virtual public Common
{
public:
  Fan_PWM(const __FlashStringHelper *Name, Module *Parent, Settings::Fan_PWMSettings *DefaultSettings);
  void refresh_Minute();
  void report(bool JSONReport = false);
  uint8_t getSpeed(bool CurrentSpeed = true);  //Returns the speed of the fan, if CurrentSpeed is true it takes into account if the fan is ON or OFF
  char *getSpeedText(bool CurrentSpeed = true, bool IncludeUnits = true);

private:
protected:
  Module *Parent;
  dimmerLamp *PWMDimmer; //Robodyn AC PWM module (with confusing dimmerLamp class name...)
  bool *State;  //ON or OFF
  uint8_t *MinSpeed; //Limit the lowest output (%)
  uint8_t *Speed;  //Speed between 0-100 (%)  (Real output mapped between MinSpeed - 100)
  uint8_t *ZeroCrossingPin;  // On Arduino Mega2560 and Nano this has to be D2 pin
  uint8_t *PWMPin;  //PWM capable digital pin
  void checkState();
  void turnOff();
  void turnOn();
  void setSpeed(uint8_t NewSpeed);
};