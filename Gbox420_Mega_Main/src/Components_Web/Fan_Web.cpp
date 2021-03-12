#include "Fan_Web.h"

Fan_Web::Fan_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::FanSettings *DefaultSettings) : Common(Name), Fan(Name, Parent, DefaultSettings), Common(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToWebsiteQueue_Button(this);
}

void Fan_Web::reportToJSON()
{
  Common::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, fanSpeedToNumber());
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void Fan_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
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
    if (strcmp_P(ShortMessage, (PGM_P)F("O")) == 0)
    {
      TurnOff();
      WebServer.setArgString(getComponentName(F("S")), fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L")) == 0)
    {
      SetLowSpeed();
      WebServer.setArgString(getComponentName(F("S")), fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("H")) == 0)
    {
      SetHighSpeed();
      WebServer.setArgString(getComponentName(F("S")), fanSpeedToText());
    }
  }
}