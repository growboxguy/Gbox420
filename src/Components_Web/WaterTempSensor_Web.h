#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/WaterTempSensor.h"

///< DS18B20 water temperature sensor - website component

class WaterTempSensor_Web : public WaterTempSensor, public Common_Web
{
public:
  WaterTempSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterTempSensorSettings *DefaultSettings); ///< constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);

private:
protected:
  Module_Web *Parent;
};