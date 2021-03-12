#pragma once

#include "../Components/420Common.h"
#include "420Module_Web.h"
#include "../Components/AirPump.h"

///< AirPump controller - website component

class AirPump_Web : public AirPump, public Common
{
public:
  AirPump_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AirPumpSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Load(__attribute__((unused)) char *url){};    ///< Not used
  void websiteEvent_Field(__attribute__((unused)) char *Field){}; ///< Not used
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
};