#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/WaterPump.h"

///< Hempy bucket - website component

class WaterPump_Web : public Common_Web, public WaterPump
{
public:
  WaterPump_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterPumpSettings *DefaultSettings);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};