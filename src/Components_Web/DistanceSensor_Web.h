#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/DistanceSensor.h"

/*! 
 *  @brief  Class for the Ultrasonic distance sensor - HC-SR04 or JSN-SR04T - Website component
 */

class DistanceSensor_Web : public DistanceSensor, public Common_Web
{
public:
  DistanceSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DistanceSensorSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(__attribute__((unused)) char *Button);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};