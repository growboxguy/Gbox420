#include "HempyBucket_Web.h"

HempyBucket_Web::HempyBucket_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WaterPump *BucketPump) : Common(Name), HempyBucket(Name,Parent,DefaultSettings,BucketWeightSensor,BucketPump), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);         
  Parent->addToRefreshQueue_Minute(this); 
  Parent->addToRefreshQueue_Sec(this);  
  Parent->addToWebsiteQueue_Load(this);   
  Parent->addToWebsiteQueue_Refresh(this);   
  Parent->addToWebsiteQueue_Button(this); 
  Parent->addToWebsiteQueue_Field(this);  
}

void HempyBucket_Web::reportToJSON()
{
    Common_Web::reportToJSON(); ///Adds a curly bracket {  that needs to be closed at the end 

    strcat_P(LongMessage, (PGM_P)F("\"}"));  ///closing the curly bracket
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
    WebServer.setArgString(getComponentName(F("Offset")), toText(* BucketWeightSensor ->  Offset));
    WebServer.setArgString(getComponentName(F("Scale")), toText(* BucketWeightSensor -> Scale));
    WebServer.setArgString(getComponentName(F("Pump")), BucketPump-> getStateText());
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