#include "Sound.h"

Sound::Sound(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Pin = &DefaultSettings->Pin;
  Enabled = &DefaultSettings->Enabled;
  pinMode(*Pin, OUTPUT);
  Parent->addToRefreshQueue_Sec(this);
  logToSerials(F("Sound object created"), true, 3);
  checkEvents();
}

void Sound::report()
{
  Common::report();
  memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("State:"));
  strcat(LongMessage, getEnabledStateText()); 
  logToSerials(&LongMessage, true, 1);
}

void Sound::reportToJSON(__attribute__((unused)) bool CloseJSON)
{
  //Common::reportToJSON(LongMessage); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\""));
  strcat_P(LongMessage, (PGM_P)Name);
  strcat_P(LongMessage, (PGM_P)F("\":{"));
  strcat_P(LongMessage, (PGM_P)F("\"En\":\""));
  strcat(LongMessage, toText(*Enabled));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void Sound::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  checkEvents();
}

void Sound::checkEvents()
{
  if (*Enabled)
  {
    if (PlayOffSound)
      OffSound();
    if (PlayOnSound)
      OnSound();
  }
  PlayOnSound = false;
  PlayOffSound = false;
}

void Sound::playOnSound()
{
  PlayOnSound = true;
}

void Sound::playOffSound()
{
  PlayOffSound = true;
}

void Sound::setSoundOnOff(bool State)
{
  *Enabled = State;
  appendName(true);
  if (*Enabled)
  {
    strcat_P(ShortMessage, (PGM_P)F("ON"));
    playOnSound();
  }
  else
  {
    strcat_P(ShortMessage, (PGM_P)F("OFF"));
    playOffSound();
  }
  Parent->addToLog(ShortMessage);
}

void Sound::OnSound()
{
  if (*Enabled)
  {
    tone(*Pin, 500);
    delay(100);
    noTone(*Pin);
    tone(*Pin, 1000);
    delay(100);
    noTone(*Pin);
  }
}

void Sound::OffSound()
{
  if (*Enabled)
  {
    tone(*Pin, 1000);
    delay(100);
    noTone(*Pin);
    tone(*Pin, 500);
    delay(100);
    noTone(*Pin);
  }
}

bool Sound::getEnabledState()
{
  return *Enabled;
}

char* Sound::getEnabledStateText()
{
  return toText_enabledDisabled(*Enabled);
}