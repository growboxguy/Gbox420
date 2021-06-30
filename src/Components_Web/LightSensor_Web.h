#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/LightSensor.h"

///< LM393 Light sensor - website component

class LightSensor_Web : public LightSensor, public Common_Web
{
public:
  LightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};