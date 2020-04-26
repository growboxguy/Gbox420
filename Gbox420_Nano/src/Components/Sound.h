#pragma once

#include "420Common.h"
#include "420Module.h"

///Sound feedback buzzer

class Sound : virtual public Common
{
public:
  Sound(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings);  
  void checkEvents(); 
  void refresh_Sec();
  void playOnSound();
  void playOffSound();

private:
 
protected:
  Module *Parent;
  void setSoundOnOff(bool State);
  void OnSound();
  void OffSound();
  byte *Pin;                 ///Pin connecting the piezo Sound positive(+) side over a 330Ω resistor)
  bool *Enabled;             ///Sound ON or OFF
  bool PlayOnSound = true;  ///Play on beep flag - website controls it
  bool PlayOffSound = true; ///Play off beep flag - website controls it

};