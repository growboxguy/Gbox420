#include "WeightSensor_Web.h"

WeightSensor_Web::WeightSensor_Web(const __FlashStringHelper *Name, Module *Parent, Settings::WeightSensorSettings *DefaultSettings) : WeightSensor(Name,Parent,DefaultSettings)
{
  Parent->AddToWebsiteQueue_Load(this);   //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->AddToWebsiteQueue_Refresh(this);   //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  Parent->AddToWebsiteQueue_Field(this);  //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
}

void WeightSensor_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/Settings.html.json") == 0)
  {
    //
  }
}

void WeightSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/Settings.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("TareOffset")), toText(*TareOffset));
    WebServer.setArgString(getWebsiteComponentName(F("Scale")), toText(*Scale));
  }
}

void WeightSensor_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Tare")) == 0)
    {
      triggerTare();
    }
  }
}

void WeightSensor_Web::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Calibrate")) == 0)
    {
      triggerCalibration(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Scale")) == 0)
    {
      setScale(WebServer.getArgFloat());
    }
  }
}