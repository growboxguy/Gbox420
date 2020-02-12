#pragma once

#include "420Common_Web.h"
#include "../Modules/420Module_Web.h"
#include "../Components/WaterLevelSensor.h"

class WaterLevelSensor_Web : public WaterLevelSensor, public Common_Web
{
public:
  WaterLevelSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterLevelSensorSettings *DefaultSettings); //constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Load(__attribute__((unused)) char *url){};  //Not used
  void websiteEvent_Button(__attribute__((unused)) char *Button){};  //Not used
  void websiteEvent_Field(__attribute__((unused)) char *Field){};  //Not used
  
private:
  
protected:
  Module_Web *Parent;
};