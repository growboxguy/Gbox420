#pragma once

#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/Aeroponics_NoTank.h"

///< Aeroponics tote without pressure tank - website component

class Aeroponics_NoTank_Web : public Aeroponics_NoTank, public Common_Web
{
public:
  Aeroponics_NoTank_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump); ///< constructor
  void websiteEvent_Load(char *url);
  void websiteEvent_Refresh(char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
};