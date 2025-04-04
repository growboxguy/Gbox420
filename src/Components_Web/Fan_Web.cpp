#include "Fan_Web.h"

Fan_Web::Fan_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::FanSettings *DefaultSettings) : Common(Name), Common_Web(Name), Fan(Name, Parent, DefaultSettings), Parent(Parent)
{
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void Fan_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("S"), true), fanSpeedText(true));
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
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L")) == 0)
    {
      SetLowSpeed();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("H")) == 0)
    {
      SetHighSpeed();
    }
    return true;
  }
}