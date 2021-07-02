#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/WaterLevelSensor.h"

///< Contactless water sensor array (4 levels) - website component

class WaterLevelSensor_Web : public WaterLevelSensor, public Common_Web
{
public:
  WaterLevelSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterLevelSensorSettings *DefaultSettings); ///< constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);

private:
protected:
  Module_Web *Parent;
};