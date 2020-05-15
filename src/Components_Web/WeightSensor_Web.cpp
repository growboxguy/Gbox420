#include "WeightSensor_Web.h"

WeightSensor_Web::WeightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WeightSensorSettings *DefaultSettings) : Common(Name), WeightSensor(Name,Parent,DefaultSettings), Common_Web(Name)
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

void WeightSensor_Web::reportToJSON()
{
    Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end 

    strcat_P(LongMessage, (PGM_P)F("\"}"));  ///< closing the curly bracket
}

void WeightSensor_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/S",2) == 0)
  {
    ///
  }
}

void WeightSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/S",2) == 0) ////When the settings page is refreshed
  {
    WebServer.setArgString(getComponentName(F("TareOffset")), toText(*TareOffset));
    WebServer.setArgString(getComponentName(F("Scale")), toText(*Scale));

  }
  else if(strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgFloat(getComponentName(F("W")), getWeight());
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