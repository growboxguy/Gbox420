#include "Sound.h"

Sound::Sound(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Parent->SoundFeedback = this; ///< Pointer for child objects to use sound feedback
  Pin = &DefaultSettings->Pin;
  Enabled = &DefaultSettings->Enabled;
  Tone1 = new TonePlayer(TCCR1A, TCCR1B, OCR1AH, OCR1AL, TCNT1H, TCNT1L);  // pin D9 (Uno/Nano), D11 (Mega)
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
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
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

void Sound::playOnOffSound(bool State)
{
  if (State)
    PlayOnSound = true;
  else
    PlayOffSound = true;
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
  playOnOffSound(*Enabled);
  Parent->addToLog(getName(getEnabledStateText(true)));
}

void Sound::OnSound()
{
  if (*Enabled)
  {
    Tone1->tone(500);
    delay(100);
    Tone1->noTone();
    Tone1->tone(1000);
    delay(100);
    Tone1->noTone();
  }
}

void Sound::OffSound()
{
  if (*Enabled)
  {
    Tone1->tone(1000);
    delay(100);
    Tone1->noTone();
    Tone1->tone(500);
    delay(100);
    Tone1->noTone();
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