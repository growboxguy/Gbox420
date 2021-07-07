#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/Fan_PWM.h"

///< PWM AC Fan controller - website component

class Fan_PWM_Web : public Fan_PWM, public Common_Web
{
public:
  Fan_PWM_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::Fan_PWMSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);
  void websiteEvent_Load(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};