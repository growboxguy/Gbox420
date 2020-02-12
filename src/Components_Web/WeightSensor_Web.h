#pragma once

#include "420Common_Web.h"
#include "../Modules/420Module_Web.h"
#include "../Components/WeightSensor.h"

class WeightSensor_Web : public WeightSensor, public Common_Web
{
public:
  WeightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WeightSensorSettings *DefaultSettings);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  
private:
  
protected:
  Module_Web *Parent;
};