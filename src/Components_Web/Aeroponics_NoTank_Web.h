#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/Aeroponics_NoTank.h"

///< Aeroponics tote without pressure tank - website component

class Aeroponics_NoTank_Web : public Aeroponics_NoTank, public Common_Web
{
public:
  Aeroponics_NoTank_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump); ///< constructor
  void websiteEvent_Load(char *Url);
  void websiteEvent_Refresh(char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};