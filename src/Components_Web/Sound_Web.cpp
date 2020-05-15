#include "Sound_Web.h"

Sound_Web::Sound_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::SoundSettings *DefaultSettings) : Common(Name), Sound(Name,Parent,DefaultSettings), Common_Web(Name) 
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToRefreshQueue_Sec(this);      
  Parent->addToWebsiteQueue_Load(this);   
  Parent->addToWebsiteQueue_Button(this); 
  Parent->addToWebsiteQueue_Field(this);   
}

void Sound_Web::reportToJSON()
{
    Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end 

    strcat_P(LongMessage, (PGM_P)F("\"Enabled\":\""));
    strcat(LongMessage, toText(*Enabled));    
    strcat_P(LongMessage, (PGM_P)F("\"}"));  ///< closing the curly bracket
}

void Sound_Web::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  if (PlayEE)
  {
    PlayEE = false;
    EE();
  }
  Sound::checkEvents();
}


void Sound_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/S",2) == 0)
  {
    WebServer.setArgBoolean(getComponentName(F("Enabled")), *Enabled);
  }
}

void Sound_Web::websiteEvent_Button(char *Button)
{ ///When a button is pressed on the website
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

void Sound_Web::websiteEvent_Field(char *Field)
{ ///When the website field is submitted
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

///EE Section, can delete everything below if you need to save space

void Sound_Web::playEE()
{
  PlayEE = true;
}

void Sound_Web::EE()
{
  Parent->addToLog(F("♬Easter egg♬"));
  for (int thisNote = 0; thisNote < 134; thisNote++)
  {
    int noteDuration = 1000 / (uint8_t)pgm_read_word(&tempo[thisNote]); ///tempo is stored in PROGMEM (Flash), cannot read from it as RAM array (temp[thisNote] would not work) ///https:///forum.arduino.cc/index.php?topic=106603.0
    buzz((int)pgm_read_word(&melody[thisNote]), noteDuration);
    delay(noteDuration);
    buzz(0, noteDuration);
    wdt_reset(); ///Reset Watchdog timeout to avoid Arduino reseting while playing the song
  }
}


void Sound_Web::buzz(uint32_t frequency, uint32_t length)
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

const PROGMEM int Sound_Web::melody[] = { ///https:///www.arduino.cc/reference/en/language/variables/utilities/progmem/
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

const PROGMEM uint8_t Sound_Web::tempo[] = {
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