#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/Fan.h"

///< Fan controller (2 speed) - website component

class Fan_Web : public Common_Web, public Fan
{
public:
  Fan_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::FanSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};