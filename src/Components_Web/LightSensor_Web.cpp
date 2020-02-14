#include "LightSensor_Web.h"

LightSensor_Web::LightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource) : LightSensor(Name,Parent,DefaultSettings,LightSource), Common(Name), Common_Web(Name)
{ //constructor
  this->Parent = Parent;
  this->Name = Name; 
  Parent->addToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Minute(this);  //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  Parent->addToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->addToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
}

void LightSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("D")), getDarkText(true));
    WebServer.setArgString(getComponentName(F("R")), getReadingText(false));
  }
}

void LightSensor_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("C")) == 0)
    {
      triggerCalibration();
    }
  }
}