#include "DHTSensor_Web.h"

DHTSensor_Web::DHTSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DHTSensorSettings *DefaultSettings) : Common(Name), DHTSensor(Name, Parent, DefaultSettings), Common(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
}



void DHTSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{ ///< When the website is refreshing
  if (strncmp(url, "/G", 2) == 0)
  {

  }
}
