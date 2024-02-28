#include "Sound_Web.h"

Sound_Web::Sound_Web(const char *Name, Module_Web *Parent, Settings::SoundSettings *DefaultSettings) : Common(Name), Common_Web(Name), Sound(Name, Parent, DefaultSettings)
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

void Sound_Web::playEE()
{
  PlayEE = true;
}

void Sound_Web::EE()
{
  Parent->addToLog("♬Easter egg♬");
  /*
  for (int thisNote = 0; thisNote < 134; thisNote++)
  {
    int noteDuration = 1000 / (uint8_t)pgm_read_word(&tempo[thisNote]); ///< tempo is stored in PROGMEM (Flash), cannot read from it as RAM array (temp[thisNote] would not work) ///< https://forum.arduino.cc/index.php?topic=106603.0
    buzz((int)pgm_read_word(&melody[thisNote]), noteDuration);
    delay(noteDuration);
    buzz(0, noteDuration);
    //wdt_reset(); ///< Reset Watchdog timeout to avoid Arduino reseting while playing the song
  }
  */
}

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

const int Sound_Web::melody[] = { ///< https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
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

const uint8_t Sound_Web::tempo[] = {
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