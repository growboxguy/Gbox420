#include "WaterTempSensor_Web.h"

WaterTempSensor_Web::WaterTempSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterTempSensorSettings *DefaultSettings) : Common(Name), WaterTempSensor(Name, Parent, DefaultSettings), Common_Web(Name), Parent(Parent)
{ 
  Parent->addToWebsiteQueue_Refresh(this);
}

void WaterTempSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("T")), getTempText(true));
}