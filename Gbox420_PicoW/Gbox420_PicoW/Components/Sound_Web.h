#pragma once

#include "420Common_Web.h"
#include "420Module_Web.h"
#include "Sound.h"

///< Sound feedback buzzer - website component

class Sound_Web : public Common_Web, public Sound
{
public:
  Sound_Web(Module_Web *Parent, Settings::SoundSettings *DefaultSettings);
  void websiteEvent_Load(char *Url);
  void websiteEvent_Field(char *Field);
  bool commandEvent(char *Command, char *Data);
  void refresh();

private:
protected:
  Module_Web *Parent;
  void buzz(uint32_t frequency, uint32_t length);
};