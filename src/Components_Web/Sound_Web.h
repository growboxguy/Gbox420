#pragma once

#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/Sound.h"


class Sound_Web : public Sound, public Common_Web
{
public:
  Sound_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::SoundSettings *DefaultSettings);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Field(char *Field);
  void websiteEvent_Button(char *Button); 
  void websiteEvent_Refresh(__attribute__((unused)) char *url){};  ///Not used
 
private: 

protected:
  Module_Web *Parent;
};