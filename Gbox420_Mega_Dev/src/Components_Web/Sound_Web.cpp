#include "Sound_Web.h"

Sound_Web::Sound_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::SoundSettings *DefaultSettings) : Common(Name), Common_Web(Name), Sound(Name, Parent, DefaultSettings)
{
  this->Parent = Parent;
  Parent->SoundFeedback = this;
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToCommandQueue(this);
}

void Sound_Web::refresh_Sec()
{
  Sound::refresh_Sec();
  if (PlayEE)
  {
    PlayEE = false;
    EE();
  }
}

void Sound_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  WebServer.setArgBoolean(getName(F("E"), true), *Enabled);
}

bool Sound_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{ ///< When the website field is submitted
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("E")) == 0)
    {
      setSoundOnOff(WebServer.getArgBoolean());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Ee")) == 0)
    {
      playEE();
    }
    return true;
  }
}

///< EE Section, can delete everything below if you need to save space

void Sound_Web::playEE()
{
  PlayEE = true;
}

void Sound_Web::EE()
{
  Parent->addToLog(F("♬Easter egg♬"));
  for (int thisNote = 0; thisNote < 134; thisNote++)
  {
    buzz((int)pgm_read_word(&melody[thisNote]), (int)pgm_read_word(&noteDurations[thisNote]));
    delay((int)pgm_read_word(&noteDurations[thisNote]));
    buzz(0, (int)pgm_read_word(&noteDurations[thisNote]));
    wdt_reset(); ///< Reset Watchdog timeout to avoid Arduino reseting while playing the song
  }
}

const PROGMEM int Sound_Web::melody[] = { ///< https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
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

const PROGMEM uint8_t Sound_Web::noteDurations[] = {
    83, 83, 83, 83, 
    83, 83, 83, 83,
    83, 83, 83, 83,
    83, 83, 83, 83,
    
    83, 83, 83, 83,
    83, 83, 83, 83,
    83, 83, 83, 83,
    83, 83, 83, 83,

    111, 111, 111,  
    83, 83, 83, 83,
    83, 83, 83, 83,
    83, 83, 83, 83,

    83, 83, 83, 83,
    83, 83, 83, 83,
    83, 83, 83, 83,
    83, 83, 83, 83,

    111, 111, 111,
    83, 83, 83, 83,
    83, 83, 83, 83,
    83, 83, 83, 83,

    33, 33, 33,   

    83, 83, 83, 83,
    83, 83, 166,    
    255,            
    83, 83, 83, 83,
    83, 83, 166,
    255,
    83, 83, 83, 83,
    83, 83, 166,
    255,
    83, 83, 83, 83,
    83, 83, 166,
    166, 55, 55, 55,  
    166, 166,
    166, 166,
    166, 166,
    55, 55, 55, 55, 55, 55,
    100, 100, 100, 
    100, 100, 100
};