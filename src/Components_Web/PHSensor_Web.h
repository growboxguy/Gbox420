#pragma once

#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/PHSensor.h"

class PHSensor_Web : public PHSensor, public Common_Web
{
public:
 PHSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::PHSensorSettings *DefaultSettings);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  
private:
  
protected:
  Module_Web *Parent;
};