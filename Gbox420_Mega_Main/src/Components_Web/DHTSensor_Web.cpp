#include "DHTSensor_Web.h"

DHTSensor_Web::DHTSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DHTSensorSettings *DefaultSettings) : Common(Name), DHTSensor(Name,Parent,DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Sensor = new DHT(*(&DefaultSettings->Pin), *(&DefaultSettings->Type)); 
  Parent->addToWebsiteQueue_Refresh(this); 
  Parent->addToReportQueue(this);          
  Parent->addToRefreshQueue_FiveSec(this);
  
}

void DHTSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{ ///When the website is refreshing
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("T")), getTempText(true)); ///Shows the latest reading
    WebServer.setArgString(getComponentName(F("H")), getHumidityText(true));
  }
}
