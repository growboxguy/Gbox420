#pragma once

#include "../Components/420Common.h"
#include "420Module_Web.h"
#include "../Components/Fan.h"

///< Fan controller (2 speed) - website component

class Fan_Web : public Fan, public Common
{
public:
  Fan_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::FanSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Load(__attribute__((unused)) char *url){};    ///< Not used
  void websiteEvent_Field(__attribute__((unused)) char *Field){}; ///< Not used
  

private:
protected:
  Module_Web *Parent;
};