#include "Fan_PWM_Web.h"

Fan_PWM_Web::Fan_PWM_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::Fan_PWMSettings *DefaultSettings) : Common(Name), Fan_PWM(Name, Parent, DefaultSettings), Common(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToWebsiteQueue_Button(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Field(this);
}



void Fan_PWM_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    
  }
}

void Fan_PWM_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    
  }
}

void Fan_PWM_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    
  }
}

void Fan_PWM_Web::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
   
  }
}