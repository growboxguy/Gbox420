#pragma once

#include "420Common.h"
#include "420Module.h"

///< Sound feedback buzzer

class Sound : virtual public Common
{
public:
  Sound(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings);
  void report(bool FriendlyFormat = false);
  void checkEvents();
  void refresh_Sec();
  void playOnSound();
  void playOffSound();
  void playOnOffSound(bool State);
  void setSoundOnOff(bool State);
  bool getEnabledState();
  char *getEnabledStateText(bool FriendlyFormat = false);

private:
protected:
  Module *Parent;
  void OnSound();
  void OffSound();
  void buzz(uint32_t frequency, uint32_t length);
  uint8_t &Pin;             ///< Pin connecting the piezo Sound positive(+) side over a 330Ω resistor)
  bool &Enabled;            ///< Sound ON or OFF
  bool PlayOnSound = true;  ///< Play on beep flag - website controls it
  bool PlayOffSound = true; ///< Play off beep flag - website controls it
};