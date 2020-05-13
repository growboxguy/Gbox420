#include "DistanceSensor_Web.h"

DistanceSensor_Web::DistanceSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DistanceSensorSettings *DefaultSettings) : Common(Name), DistanceSensor(Name,Parent,DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name; 
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToWebsiteQueue_Refresh(this);   
}

void DistanceSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{ ///When the website is refreshing
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Dis")), getDistanceText(true)); ///Distance reading
  }
}

void DistanceSensor_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    //if (strcmp_P(ShortMessage, (PGM_P)F("Emp")) == 0){setEmptyDistance();}    ///Read the distance when the reservoir is empty
    
  }
}