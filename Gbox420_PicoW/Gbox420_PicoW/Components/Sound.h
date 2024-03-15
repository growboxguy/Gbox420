/*!
 *  \brief     Sound feedback using a piezo buzzer  
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

#pragma once

#include "420Common.h"
#include "420Module.h"
#include "hardware/gpio.h"

///< Sound feedback buzzer

class Sound : virtual public Common
{
public:
  Sound(Module *Parent, Settings::SoundSettings *DefaultSettings);
  void report(bool FriendlyFormat = false);
  bool commandEvent(char *Command, char *Data);
  void checkEvents();
  void run1sec();
  void playOnSound();
  void playOffSound();
  void playEE();
  void buzz(uint32_t frequency, uint32_t length);
  void playOnOffSound(bool State);
  void setSoundOnOff(bool State);
  void toggleSoundOnOff();
  bool getEnabledState();
  char *getEnabledStateText(bool FriendlyFormat = false);
  // void websiteEvent_Load(char *Url);
  // void websiteEvent_Field(char *Field);

private:
protected:
  Module *Parent;
  void OnSound();
  void OffSound();
  void EE();
  uint8_t *Pin;             ///< Pin connecting the piezo Sound positive(+) side over a 330Ω resistor)
  bool *Enabled;            ///< Sound ON or OFF
  bool PlayOnSound = true;  ///< Play on beep flag - website controls it
  bool PlayOffSound = true; ///< Play off beep flag - website controls it
  bool PlayEE = false;
  int slice_num;

  const static uint16_t melody[];
  const static uint8_t tempo[];
};