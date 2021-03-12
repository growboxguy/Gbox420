#include "AirPump_Web.h"

AirPump_Web::AirPump_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AirPumpSettings *DefaultSettings) : Common(Name), AirPump(Name, Parent, DefaultSettings), Common(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToWebsiteQueue_Button(this);
}



void AirPump_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    
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
    
  }
}