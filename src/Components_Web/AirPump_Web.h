#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/AirPump.h"

///< AirPump controller - website component

class AirPump_Web : public Common_Web, public AirPump
{
public:
  AirPump_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AirPumpSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};