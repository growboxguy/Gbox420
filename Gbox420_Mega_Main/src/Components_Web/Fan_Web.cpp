#include "Fan_Web.h"

Fan_Web::Fan_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::FanSettings *DefaultSettings) : Common(Name), Common_Web(Name), Fan(Name, Parent, DefaultSettings)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void Fan_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  WebServer.setArgString(getName(F("S"),true), fanSpeedText());
}

bool Fan_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("O")) == 0)
    {
      TurnOff();
      WebServer.setArgString(getName(F("S")), fanSpeedText(true));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L")) == 0)
    {
      SetLowSpeed();
      WebServer.setArgString(getName(F("S")), fanSpeedText(true));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("H")) == 0)
    {
      SetHighSpeed();
      WebServer.setArgString(getName(F("S")), fanSpeedText(true));
    }
    return true;
  }
}