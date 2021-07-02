#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/WeightSensor.h"

///< Weight sensor load cell - website component

class WeightSensor_Web : public WeightSensor, public Common_Web
{
public:
  WeightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WeightSensorSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};