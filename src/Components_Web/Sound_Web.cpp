#include "Sound_Web.h"

Sound_Web::Sound_Web(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings) : Sound(Name,Parent,DefaultSettings)
{
  this->Parent = Parent;  
  this->Parent->AddToWebsiteQueue_Load(this);   //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  this->Parent->AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  this->Parent->AddToWebsiteQueue_Field(this);  //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  this->Parent->addToRefreshQueue_Sec(this);    //Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method  
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