#include "Lights_Web.h"

Lights_Web::Lights_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightsSettings *DefaultSettings) : Common(Name), Lights(Name, Parent, DefaultSettings), Common(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToWebsiteQueue_Field(this);
  Parent->addToWebsiteQueue_Button(this);
}



void Lights_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
   
  }
}

void Lights_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    
  }
}

void Lights_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
   
  }
}

void Lights_Web::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    
  }
}