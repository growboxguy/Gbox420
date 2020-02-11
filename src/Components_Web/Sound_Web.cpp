#include "Sound_Web.h"

Sound_Web::Sound_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::SoundSettings *DefaultSettings) : Sound(Name,Parent,DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToWebsiteQueue_Load(this);   //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->addToWebsiteQueue_Button(this); //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  Parent->addToWebsiteQueue_Field(this);  //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method 
}

void Sound_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/Settings.html.json") == 0)
  {
    WebServer.setArgBoolean(getComponentName(F("Enabled")), *Enabled);
  }
}

void Sound_Web::websiteEvent_Button(char *Button)
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

void Sound_Web::websiteEvent_Field(char *Field)
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