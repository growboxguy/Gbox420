#pragma once

#include "420Common.h"
#include "420Module.h"
#include "hardware/pwm.h"
// #include "TonePlayer.h"  ///< http://www.gammon.com.au/forum/?id=11504&reply=11#reply11

///< Sound feedback buzzer

class Sound : virtual public Common
{
public:
  Sound(Module *Parent, Settings::SoundSettings *DefaultSettings);
  void report(bool FriendlyFormat = false);
  bool commandEvent(char *Command, char *Data);
  void checkEvents();
  void refresh();
  void playOnSound();
  void playOffSound();
  void playEE();
  void inline pwm_calcDivTop(pwm_config *c, int frequency, int sysClock);
  void beep(int note, int duration); // Generate a PWM signal at the specified frequency for the specified duration
  void playOnOffSound(bool State);
  void setSoundOnOff(bool State);
  bool getEnabledState();
  char *getEnabledStateText(bool FriendlyFormat = false);

private:
protected:
  Module *Parent;
  // TonePlayer *Tone1;
  void OnSound();
  void OffSound();
  void EE();
  uint8_t *Pin;             ///< Pin connecting the piezo Sound positive(+) side over a 330Ω resistor)
  bool *Enabled;            ///< Sound ON or OFF
  bool PlayOnSound = true;  ///< Play on beep flag - website controls it
  bool PlayOffSound = true; ///< Play off beep flag - website controls it
  bool PlayEE = false;
  pwm_config cfg;
  int slice_num;

  const static int melody[];
  const static uint16_t tempo[];
};