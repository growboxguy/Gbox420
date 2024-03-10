#include "Sound_Web.h"

Sound_Web::Sound_Web(Module_Web *Parent, Settings::SoundSettings *DefaultSettings) : Common(DefaultSettings->Name), Common_Web(DefaultSettings->Name), Sound(Parent, DefaultSettings)
{
  this->Parent = Parent;
  Parent->SoundFeedback = this;
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToCommandQueue(this);
}

void Sound_Web::refresh()
{
  Sound::refresh();
}

void Sound_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  // WebServer.setArgBoolean(getName("E"), true), *Enabled);
}

bool Sound_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{ ///< When the website field is submitted
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp(ShortMessage, "E") == 0)
    {
      // setSoundOnOff(WebServer.getArgBoolean());
    }
    else if (strcmp(ShortMessage, "Ee") == 0)
    {
      playEE();
    }
    return true;
  }
}

///< EE Section, can delete everything below if you need to save space

void Sound_Web::buzz(uint32_t frequency, uint32_t length)
{
  /*
  digitalWrite(13, HIGH);
  uint32_t delayValue = 1000000 / frequency / 2;
  uint32_t numCycles = frequency * length / 1000;
  for (uint32_t i = 0; i < numCycles; i++)
  {
    digitalWrite(*Pin, HIGH);
    delayMicroseconds(delayValue);
    digitalWrite(*Pin, LOW);
    delayMicroseconds(delayValue);
  }
  digitalWrite(13, LOW);
  */
}
