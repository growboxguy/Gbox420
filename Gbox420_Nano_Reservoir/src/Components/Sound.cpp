#include "Sound.h"

Sound::Sound(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Pin = &DefaultSettings->Pin;
  Enabled = &DefaultSettings->Enabled;
  pinMode(*Pin, OUTPUT);
  Parent->addToRefreshQueue_Sec(this);
  logToSerials(F("Sound ready"), true, 3);
  checkEvents();
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void Sound::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"En\":\""));
  strcat(LongMessage, getEnabledStateText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void Sound::refresh_Sec()
{
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
  if (*Enabled)
  {
    playOnSound();
    strcat(ShortMessage, getName(F("ON")));
  }
  else
  {
    playOffSound();
    strcat(ShortMessage, getName(F("OFF")));
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

char *Sound::getEnabledStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_enabledDisabled(*Enabled);
  }
  else
  {
    return toText(*Enabled);
  }
}