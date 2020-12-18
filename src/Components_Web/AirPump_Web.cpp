#include "AirPump_Web.h"

AirPump_Web::AirPump_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AirPumpSettings *DefaultSettings) : Common(Name), AirPump(Name, Parent, DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToWebsiteQueue_Button(this);
}

void AirPump_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"Stat\":\""));
  strcat(LongMessage, toText(getState()));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void AirPump_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Stat")), getStateToText());
  }
}

void AirPump_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
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