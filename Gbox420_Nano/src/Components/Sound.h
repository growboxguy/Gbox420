#pragma once

#include "420Common.h"
#include "420Module.h"

///Sound feedback buzzer

class Sound : virtual public Common
{
public:
  Sound(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings);  
  void refresh_Sec();
  void playOnSound();
  void playOffSound();
  void playEE();

private:
 
protected:
  Module *Parent;
  void setSoundOnOff(bool State);
  void OnSound();
  void OffSound();
  void buzz(uint32_t frequency, uint32_t length);
  void EE();
  byte *Pin;                 ///Pin connecting the piezo Sound positive(+) side over a 330Ω resistor)
  bool *Enabled;             ///Sound ON or OFF
  bool PlayOnSound = false;  ///Play on beep flag - website controls it
  bool PlayOffSound = false; ///Play off beep flag - website controls it
  bool PlayEE = false;       ///Surprise :) - website controls it
  const PROGMEM static int melody[];
  const PROGMEM static byte tempo[];
};