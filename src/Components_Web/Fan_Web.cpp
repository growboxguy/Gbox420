#include "Fan_Web.h"

Fan_Web::Fan_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::FanSettings *DefaultSettings) : Fan(Name,Parent,DefaultSettings), Common_Web(Name), Common(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Minute(this);  //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method 
  Parent->addToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->addToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
}

void Fan_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("S")), fanSpeedToText());
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
    if (strcmp_P(ShortMessage, (PGM_P)F("O")) == 0){TurnOff();WebServer.setArgString(getComponentName(F("S")), fanSpeedToText());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("L")) == 0){SetLowSpeed();WebServer.setArgString(getComponentName(F("S")), fanSpeedToText());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("H")) == 0){SetHighSpeed(); WebServer.setArgString(getComponentName(F("S")), fanSpeedToText());
    }
  }
}