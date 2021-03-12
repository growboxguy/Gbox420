#pragma once

#include "../Components/420Common.h"
#include "420Module_Web.h"
#include "../Components/Fan_PWM.h"

///< PWM AC Fan controller - website component

class Fan_PWM_Web : public Fan_PWM, public Common
{
public:
  Fan_PWM_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::Fan_PWMSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Load(__attribute__((unused)) char *url);    
  void websiteEvent_Field(__attribute__((unused)) char *Field);
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
};