#pragma once

#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/DistanceSensor.h"

/*! 
 *  @brief  Class for the Ultrasonic distance sensor - HC-SR04 or JSN-SR04T - Website component
 */

class DistanceSensor_Web : public DistanceSensor, public Common_Web
{
public:
  DistanceSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DistanceSensorSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Load(__attribute__((unused)) char *url){}; ///Not used
  void websiteEvent_Button(__attribute__((unused)) char *Button);
  void websiteEvent_Field(__attribute__((unused)) char *Field){}; ///Not used
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
};