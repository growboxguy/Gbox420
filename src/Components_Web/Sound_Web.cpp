#include "Sound.h"
#include "GrowBox.h"

Sound::Sound(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings) : Fan(Name,Parent,DefaultSettings)
{
  this->Parent = Parent;  
  Parent->AddToWebsiteQueue_Load(this);   //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  Parent->AddToWebsiteQueue_Field(this);  //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  Parent->AddToRefreshQueue_Sec(this);    //Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method  
}

void Sound::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/Settings.html.json") == 0)
  {
    WebServer.setArgBoolean(getWebsiteComponentName(F("Enabled")), *Enabled);
  }
}

void Sound::websiteEvent_Button(char *Button)
{ //When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Ee")) == 0)
    {
      playEE();
    }
  }
}

void Sound::websiteEvent_Field(char *Field)
{ //When the website field is submitted
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Enabled")) == 0)
    {
      setSoundOnOff(WebServer.getArgBoolean());
    }
  }
}

void Sound::refresh_Sec()
{
  if (*DebugEnabled)
    Common_Web::refresh_Sec();
  if (*Enabled)
  {
    if (PlayOnSound)
      OnSound();
    if (PlayOffSound)
      OffSound();
  }
  PlayOnSound = false;
  PlayOffSound = false;
  if (PlayEE)
  {
    PlayEE = false;
    EE();
  }
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

//EE Section, can delete everything below if you need to save space

void Sound::playEE()
{
  PlayEE = true;
}

const PROGMEM int Sound::melody[] = { //https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
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

const PROGMEM byte Sound::tempo[] = {
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

void Sound::EE()
{
  Parent->addToLog(F("♬Easter egg♬"));
  for (int thisNote = 0; thisNote < 134; thisNote++)
  {
    int noteDuration = 1000 / (byte)pgm_read_word(&tempo[thisNote]); //tempo is stored in PROGMEM (Flash), cannot read from it as RAM array (temp[thisNote] would not work) //https://forum.arduino.cc/index.php?topic=106603.0
    buzz((int)pgm_read_word(&melody[thisNote]), noteDuration);
    delay(noteDuration);
    buzz(0, noteDuration);
    wdt_reset(); //Reset Watchdog timeout to avoid Arduino reseting while playing the song
  }
}

void Sound::buzz(uint32_t frequency, uint32_t length)
{
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
}