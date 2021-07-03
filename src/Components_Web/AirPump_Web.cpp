#include "AirPump_Web.h"

AirPump_Web::AirPump_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AirPumpSettings *DefaultSettings) : Common(Name), Common_Web(Name), AirPump(Name, Parent, DefaultSettings)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void AirPump_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("S"),true), getStateText(true));
}

bool AirPump_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("On")) == 0)
    {
      TurnOn();
      Parent->addToLog(getName(getStateText(true)), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Of")) == 0)
    {
      TurnOff();
      Parent->addToLog(getName(getStateText(true)), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("S")) == 0)
    {
      setState(toBool(Data));
      Parent->addToLog(getName(getStateText(true)), false);
    }
    return true;
  }
}