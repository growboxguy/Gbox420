#include "WaterTempSensor_Web.h"
#include "GrowBox.h"

WaterTempSensor_Web::WaterTempSensor_Web(const __FlashStringHelper *Name, Module *Parent, Settings::WaterTempSensorSettings *DefaultSettings) : WaterTempSensor(Name,Parent,DefaultSettings)
{ //constructor
  Parent->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
}

void WaterTempSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Temp")), getTempText(true, false));
  }
}