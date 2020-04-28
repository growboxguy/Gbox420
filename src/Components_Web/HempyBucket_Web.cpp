#include "HempyBucket_Web.h"

HempyBucket_Web::HempyBucket_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WaterPump *BucketPump) : HempyBucket(Name,Parent,DefaultSettings,BucketWeightSensor,BucketPump), Common_Web(Name), Common(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);         ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Minute(this); ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  Parent->addToRefreshQueue_Sec(this);  
  Parent->addToWebsiteQueue_Load(this);   ///Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->addToWebsiteQueue_Refresh(this);   ///Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->addToWebsiteQueue_Button(this); ///Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  Parent->addToWebsiteQueue_Field(this);  ///Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
}

void HempyBucket_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/S",2) == 0)
  {
    ///
  }
}

void HempyBucket_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/S",2) == 0) ////When the settings page is refreshed
  {
    WebServer.setArgString(getComponentName(F("TareOffset")), toText(* BucketWeightSensor ->  TareOffset));
    WebServer.setArgString(getComponentName(F("Scale")), toText(* BucketWeightSensor -> Scale));
    WebServer.setArgString(getComponentName(F("Pump")), BucketPump-> getState());
  }
  else if(strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgFloat(getComponentName(F("W")), BucketWeightSensor -> getWeight());
  }
}

void HempyBucket_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Tare")) == 0)
    {
      BucketWeightSensor -> triggerTare();
    }
  }
}

void HempyBucket_Web::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Calibrate")) == 0)
    {
      BucketWeightSensor ->triggerCalibration(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Scale")) == 0)
    {
      BucketWeightSensor -> setScale(WebServer.getArgFloat());
    }
  }
}