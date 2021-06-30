#include "Fan_Web.h"

Fan_Web::Fan_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::FanSettings *DefaultSettings) : Common(Name), Fan(Name, Parent, DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void Fan_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
    WebServer.setArgString(getName(F("S")), fanSpeedToText());
}

void Fan_Web::websiteEvent_ButtoncommandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("O")) == 0)
    {
      TurnOff();
      WebServer.setArgString(getName(F("S")), fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L")) == 0)
    {
      SetLowSpeed();
      WebServer.setArgString(getName(F("S")), fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("H")) == 0)
    {
      SetHighSpeed();
      WebServer.setArgString(getName(F("S")), fanSpeedToText());
    }
  }
}