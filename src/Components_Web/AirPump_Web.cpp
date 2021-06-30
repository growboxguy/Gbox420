#include "AirPump_Web.h"

AirPump_Web::AirPump_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AirPumpSettings *DefaultSettings) : Common(Name), AirPump(Name, Parent, DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void AirPump_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  WebServer.setArgString(getName(F("Stat")), getStateToText());
}

void AirPump_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMine(Button))
  {
    return;
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
  }
}