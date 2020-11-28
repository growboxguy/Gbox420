#pragma once

#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/PressureSensor.h"

///< Pressure sensor - website component

class PressureSensor_Web : public PressureSensor, public Common_Web
{
public:
  PressureSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::PressureSensorSettings *DefaultSettings);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void websiteEvent_Refresh(__attribute__((unused)) char *url){}; ///< Not used
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
};