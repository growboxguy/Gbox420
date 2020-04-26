#include "DHTSensor_Web.h"

DHTSensor_Web::DHTSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DHTSensorSettings *DefaultSettings) : DHTSensor(Name,Parent,DefaultSettings), Common_Web(Name), Common(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Sensor = new DHT(*(&DefaultSettings->Pin), *(&DefaultSettings->Type)); 
  Parent->addToWebsiteQueue_Refresh(this); ///Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->addToReportQueue(this);          ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Minute(this);  ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  
}

void DHTSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{ ///When the website is refreshing
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("T")), getTempText(true)); ///Shows the latest reading
    WebServer.setArgString(getComponentName(F("H")), getHumidityText(true));
  }
}
