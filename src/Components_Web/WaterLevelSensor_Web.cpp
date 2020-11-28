#include "WaterLevelSensor_Web.h"

WaterLevelSensor_Web::WaterLevelSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterLevelSensorSettings *DefaultSettings) : Common(Name), WaterLevelSensor(Name, Parent, DefaultSettings), Common_Web(Name)
{ ///< constructor
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Refresh(this);
}

void WaterLevelSensor_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end

  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void WaterLevelSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Level")), getLevelGauge());
  }
}