#pragma once

#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/WaterTempSensor.h"

///< DS18B20 water temperature sensor - website component

class WaterTempSensor_Web : public WaterTempSensor, public Common_Web
{
public:
  WaterTempSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterTempSensorSettings *DefaultSettings); ///< constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Load(__attribute__((unused)) char *url){};      ///< Not used
  void websiteEvent_Button(__attribute__((unused)) char *Button){}; ///< Not used
  void websiteEvent_Field(__attribute__((unused)) char *Field){};   ///< Not used
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
};