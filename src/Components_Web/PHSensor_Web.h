#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/PHSensor.h"

///< PH sensor - website component

class PHSensor_Web : public PHSensor, public Common_Web
{
public:
  PHSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::PHSensorSettings *DefaultSettings);
  void websiteEvent_Load(__attribute__((unused)) char *Url);
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};