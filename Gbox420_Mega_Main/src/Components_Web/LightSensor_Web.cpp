#include "LightSensor_Web.h"

LightSensor_Web::LightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource) : Common(Name), LightSensor(Name,Parent,DefaultSettings,LightSource), Common_Web(Name)
{ ///constructor
  this->Parent = Parent;
  this->Name = Name; 
  Parent->addToReportQueue(this);         
  Parent->addToRefreshQueue_FiveSec(this);       
  Parent->addToWebsiteQueue_Refresh(this); 
  Parent->addToWebsiteQueue_Button(this);  
}

void LightSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("D")), getDarkText(true));
    WebServer.setArgString(getComponentName(F("R")), getReadingText());
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