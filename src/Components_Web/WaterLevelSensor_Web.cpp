#include "WaterLevelSensor_Web.h"

WaterLevelSensor_Web::WaterLevelSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterLevelSensorSettings *DefaultSettings) : Common(Name), WaterLevelSensor(Name, Parent, DefaultSettings), Common_Web(Name)
{ ///< constructor
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
}

void WaterLevelSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("Level")), getLevelGauge());
}