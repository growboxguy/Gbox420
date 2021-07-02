#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/PressureSensor.h"

///< Pressure sensor - website component

class PressureSensor_Web : public PressureSensor, public Common_Web
{
public:
  PressureSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::PressureSensorSettings *DefaultSettings);
  void websiteEvent_Load(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};