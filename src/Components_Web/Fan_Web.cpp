#include "Fan_Web.h"

Fan_Web::Fan_Web(const __FlashStringHelper *Name, Module *Parent, Settings::FanSettings *DefaultSettings) : Fan(Name,DefaultSettings)
{
  this->Parent = Parent; 
  Parent->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->AddToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
}

void Fan_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Status")), fanSpeedToText());
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
      WebServer.setArgString(getWebsiteComponentName(F("Status")), fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Low")) == 0)
    {
      SetLowSpeed();
      WebServer.setArgString(getWebsiteComponentName(F("Status")), fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("High")) == 0)
    {
      SetHighSpeed();
      WebServer.setArgString(getWebsiteComponentName(F("Status")), fanSpeedToText());
    }
  }
}