#include "WaterLevelSensor_Web.h"

WaterLevelSensor_Web::WaterLevelSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterLevelSensorSettings *DefaultSettings) : WaterLevelSensor(Name,Parent,DefaultSettings), Common_Web(Name)
{ //constructor
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
}

void WaterLevelSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getComponentName(F("Level")), getLevelGauge());
  }
}