#include "AirPump_Web.h"

AirPump_Web::AirPump_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AirPumpSettings *DefaultSettings) : Common(Name), Common_Web(Name), AirPump(Name, Parent, DefaultSettings)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void AirPump_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  WebServer.setArgString(getName(F("S"),true), getStateText());
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
      *State = true;
      checkStatus();
      Parent->addToLog(F("Air pump ON"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Off")) == 0)
    {
      *State = false;
      checkStatus();
      Parent->addToLog(F("Air pump OFF"), false);
    }
    return true;
  }
}