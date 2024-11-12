#include "Sound.h"

Sound::Sound(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings)
    : Common(Name), Parent(Parent), Pin(DefaultSettings->Pin), Enabled(DefaultSettings->Enabled)
{
  Parent->SoundFeedback = this; ///< Pointer for child objects to use sound feedback
  pinMode(Pin, OUTPUT);
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
  if (Enabled)
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
  Enabled = State;
  playOnOffSound(Enabled);
  Parent->addToLog(getName(getEnabledStateText(true)));
}

void Sound::OnSound()
{
  if (Enabled)
  {
    buzz(250, 100);
    buzz(0, 20);
    buzz(750, 100);
  }
}

void Sound::OffSound()
{
  if (Enabled)
  {
    buzz(750, 100);
    buzz(0, 20);
    buzz(250, 100);
  }
}

void Sound::buzz(uint32_t frequency, uint32_t length)
{
  digitalWrite(13, HIGH); // Blink built-in LED
  uint32_t delayValue = 1000000 / frequency / 2;
  uint32_t numCycles = frequency * length / 1000;
  for (uint32_t i = 0; i < numCycles; i++)
  {
    digitalWrite(Pin, HIGH);
    delayMicroseconds(delayValue);
    digitalWrite(Pin, LOW);
    delayMicroseconds(delayValue);
  }
  digitalWrite(13, LOW); // Blink built-in LED
}

bool Sound::getEnabledState()
{
  return Enabled;
}

char *Sound::getEnabledStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_enabledDisabled(Enabled);
  }
  else
  {
    return toText(Enabled);
  }
}