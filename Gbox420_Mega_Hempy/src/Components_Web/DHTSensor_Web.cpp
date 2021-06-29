#include "DHTSensor_Web.h"

DHTSensor_Web::DHTSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DHTSensorSettings *DefaultSettings) : Common(Name), Common_Web(Name), DHTSensor(Name, Parent, DefaultSettings)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
}

void DHTSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{                                                                   ///< When the website is refreshing
  WebServer.setArgString(getName(F("T"), true), getTempText(true)); ///< Shows the latest reading
  WebServer.setArgString(getName(F("H"), true), getHumidityText(true));
}
