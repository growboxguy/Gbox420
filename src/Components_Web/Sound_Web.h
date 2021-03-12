#pragma once

#include "../Components/420Common.h"
#include "420Module_Web.h"
#include "../Components/Sound.h"

///< Sound feedback buzzer - website component

class Sound_Web : public Sound, public Common
{
public:
  Sound_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::SoundSettings *DefaultSettings);
  void checkEvents();
  void refresh_Sec();
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Field(char *Field);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Refresh(__attribute__((unused)) char *url){}; ///< Not used
  void playEE();
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
  void EE();
  void buzz(uint32_t frequency, uint32_t length);
  bool PlayEE = false; ///< Surprise :) - website controls it
  const PROGMEM static int melody[];
  const PROGMEM static uint8_t tempo[];
};