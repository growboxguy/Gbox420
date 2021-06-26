#pragma once

#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/Aeroponics_Tank.h"

///< Aeroponics tote with pressure tank - website component

class Aeroponics_Tank_Web : public Aeroponics_Tank, public Common_Web
{
public:
  Aeroponics_Tank_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
};