#include "LightSensor_Web.h"
#include "GrowBox.h"

LightSensor_Web::LightSensor_Web(const __FlashStringHelper *Name, Module *Parent, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource) : Lights(Name,Parent,DefaultSettings,LightSource)
{ //constructor
  this->Parent = Parent;  
  Parent->AddToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  Parent->AddToRefreshQueue_Minute(this);  //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  Parent->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->AddToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
}

void LightSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Dark")), getDarkText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Reading")), getReadingText(false));
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
    if (strcmp_P(ShortMessage, (PGM_P)F("Calibrate")) == 0)
    {
      triggerCalibration();
    }
  }
}