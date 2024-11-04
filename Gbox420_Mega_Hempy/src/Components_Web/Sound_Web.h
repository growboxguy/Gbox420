#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/Sound.h"

///< Sound feedback buzzer - website component

class Sound_Web : public Common_Web, public Sound
{
public:
  Sound_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::SoundSettings *DefaultSettings);
  void websiteEvent_Load(char *Url);
  void websiteEvent_Field(char *Field);
  bool commandEvent(char *Command, char *Data);
  void refresh_Sec();
  void playEE();

private:
protected:
  Module_Web *Parent;
  void EE();  
  bool PlayEE = false;
  const PROGMEM static int melody[];
  const PROGMEM static uint8_t noteDurations[];
};