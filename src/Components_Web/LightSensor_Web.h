#pragma once

#include "../Components/420Common.h"
#include "420Module_Web.h"
#include "../Components/LightSensor.h"

///< LM393 Light sensor - website component

class LightSensor_Web : public LightSensor, public Common
{
public:
  LightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Load(__attribute__((unused)) char *url){};    ///< Not used
  void websiteEvent_Field(__attribute__((unused)) char *Field){}; ///< Not used
  

private:
protected:
  Module_Web *Parent;
};