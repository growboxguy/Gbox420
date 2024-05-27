#include "Sound.h"

Sound::Sound(Module *Parent, Settings::SoundSettings *DefaultSettings) : Common(DefaultSettings->Name)
{
  this->Parent = Parent;
  Parent->DefaultSound = this; ///< Pointer for child objects to use sound feedback
  Pin = &DefaultSettings->Pin;
  Enabled = &DefaultSettings->Enabled;
  gpio_init(*Pin);
  gpio_set_dir(*Pin, GPIO_OUT);
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_1sec(this);
  Parent->addToCommandQueue(this);
  printf("Sound ready\n");
  checkEvents();
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void Sound::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat(LongMessage, "\"S\":\"");
  strcat(LongMessage, getEnabledStateText(FriendlyFormat));
  strcat(LongMessage, "\"}"); ///< closing the curly bracket at the end of the JSON
}

bool Sound::commandEvent(char *Command, char *Data)
{ ///< When the website field is submitted
  if (!isThisForMe(Command))
  {
    return false;
  }
  else
  {
    if (strcmp(ShortMessage, "E") == 0)
    {
      toggleSoundOnOff();
    }
    else if (strcmp(ShortMessage, "Ee") == 0)
    {
      playEE();
    }
    return true;
  }
}

/*
void Sound::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  // WebServer.setArgBoolean(getName("E"), true), *Enabled);
}

*/

void Sound::run1sec()
{
  Common::run1sec();
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
    if (PlayEE)
      EE();
  }
  PlayOnSound = false;
  PlayOffSound = false;
  PlayEE = false;
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

void Sound::playEE()
{
  PlayEE = true;
}

void Sound::setSoundOnOff(bool State)
{
  *Enabled = State;
  playOnOffSound(*Enabled);
  Parent->addToLog(getName(getEnabledStateText(true)));
}

void Sound::toggleSoundOnOff()
{
  *Enabled = !(*Enabled);
  playOnOffSound(*Enabled);
  Parent->addToLog(getName(getEnabledStateText(true)));
}

void Sound::buzz(uint32_t frequency, uint32_t length)
{
  uint32_t delayValue = 2200000 / frequency / 2;
  uint32_t numCycles = frequency * length / 1000;
  for (uint32_t i = 0; i < numCycles; i++)
  {
    gpio_put(*Pin, true);
    busy_wait_us_32(delayValue);
    gpio_put(*Pin, false);
    busy_wait_us_32(delayValue);
  }
}

void Sound::OnSound()
{
  if (*Enabled)
  {
    buzz(500, 100);
    busy_wait_us_32(10000);
    buzz(2000, 100);
  }
}

void Sound::OffSound()
{
  if (*Enabled)
  {
    buzz(500, 100);
    busy_wait_us_32(10000);
    buzz(2000, 100);
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

///< EE Section, can delete everything below if you need to save space
void Sound::EE()
{
  printf("Easter egg\n");
  for (int thisNote = 0; thisNote < 134; thisNote++)
  {
    uint32_t noteDuration = 800 / tempo[thisNote];
    buzz(melody[thisNote], noteDuration);
    busy_wait_ms(noteDuration);
    buzz(0, noteDuration);
  }
}

const uint16_t Sound::melody[] = {
    2637, 2637, 0, 2637,
    0, 2093, 2637, 0,
    3136, 0, 0, 0,
    1568, 0, 0, 0,

    2093, 0, 0, 1568,
    0, 0, 1319, 0,
    0, 1760, 0, 1976,
    0, 1865, 1760, 0,

    1568, 2637, 3136,
    3520, 0, 2794, 3136,
    0, 2637, 0, 2093,
    2349, 1976, 0, 0,

    2093, 0, 0, 1568,
    0, 0, 1319, 0,
    0, 1760, 0, 1976,
    0, 1865, 1760, 0,

    1568, 2637, 3136,
    3520, 0, 2794, 3136,
    0, 2637, 0, 2093,
    2349, 1976, 0, 0,

    0, 0, 0,

    262, 523, 220, 440,
    233, 466, 0,
    0,
    262, 523, 220, 440,
    233, 466, 0,
    0,
    175, 349, 147, 294,
    156, 311, 0,
    0,
    175, 349, 147, 294,
    156, 311, 0,
    0, 311, 277, 294,
    277, 311,
    311, 208,
    196, 277,
    262, 370, 349, 165, 466, 440,
    415, 311, 247,
    233, 220, 208};

const uint8_t Sound::tempo[] = {
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    9, 9, 9,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    9, 9, 9,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    30, 30, 30,

    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    6, 18, 18, 18,
    6, 6,
    6, 6,
    6, 6,
    18, 18, 18, 18, 18, 18,
    10, 10, 10,
    10, 10, 10};