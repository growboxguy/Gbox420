#include "Sound.h"

Sound::Sound(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Pin = &DefaultSettings->Pin;
  Enabled = &DefaultSettings->Enabled;
  pinMode(*Pin, OUTPUT); 
  Parent->addToRefreshQueue_Sec(this);    ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method  
  logToSerials(F("Sound object created"), true, 1);
}

void Sound::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  checkEvents();
}

void Sound::checkEvents(){
  if (*Enabled)
  {
    if (PlayOnSound)
      OnSound();
    if (PlayOffSound)
      OffSound();
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
    Parent->addToLog(F("Sound enabled"));
    playOnSound();
  }
  else
  {
    Parent->addToLog(F("Sound disabled"));
    playOffSound();
  }
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