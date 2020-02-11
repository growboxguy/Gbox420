#include "Fan_Web.h"

Fan_Web::Fan_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::FanSettings *DefaultSettings) : Fan(Name,Parent,DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->addToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
}

void Fan_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getComponentName(F("Status")), fanSpeedToText());
  }
}

void Fan_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Off")) == 0)
    {
      TurnOff();
      WebServer.setArgString(getComponentName(F("Status")), fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Low")) == 0)
    {
      SetLowSpeed();
      WebServer.setArgString(getComponentName(F("Status")), fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("High")) == 0)
    {
      SetHighSpeed();
      WebServer.setArgString(getComponentName(F("Status")), fanSpeedToText());
    }
  }
}