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

void Sound::report(bool JSONReport)
{
  Common::report(JSONReport); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    strcat_P(LongMessage, (PGM_P)F("\"En\":\""));
    strcat(LongMessage, toText(*Enabled));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else //Print a report to the Serial console
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("State:"));
    strcat(LongMessage, getEnabledStateText());
    logToSerials(&LongMessage, true, 1);
  }
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
  appendName(true);
  strcat(ShortMessage, toText_onOff(*Enabled));
  if (*Enabled)
  {
    playOnSound();
  }
  else
  {
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

char *Sound::getEnabledStateText()
{
  return toText_enabledDisabled(*Enabled);
}