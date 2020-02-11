#include "WaterTempSensor_Web.h"

WaterTempSensor_Web::WaterTempSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterTempSensorSettings *DefaultSettings) : WaterTempSensor(Name,Parent,DefaultSettings)
{ //constructor
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
}

void WaterTempSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getComponentName(F("Temp")), getTempText(true, false));
  }
}