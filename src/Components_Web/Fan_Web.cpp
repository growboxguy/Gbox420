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



void Fan_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    
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
   
  }
}