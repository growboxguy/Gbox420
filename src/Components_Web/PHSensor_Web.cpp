#include "PHSensor_Web.h"

PHSensor_Web::PHSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::PHSensorSettings *DefaultSettings) : Common(Name), PHSensor(Name, Parent, DefaultSettings), Common_Web(Name)
{ ///constructor
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToWebsiteQueue_Field(this);
  Parent->addToWebsiteQueue_Button(this);
}

void PHSensor_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///Adds a curly bracket {  that needs to be closed at the end

  strcat_P(LongMessage, (PGM_P)F("\"}")); ///closing the curly bracket
}

void PHSensor_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/S", 2) == 0)
  {
    ///WebServer.setArgString(F("PHAlertLow"), toText(GBox -> Reservoir -> PHAlertLow));
    ///WebServer.setArgString(F("PHAlertHigh"), toText(GBox -> ModuleSettings -> PHAlertHigh));
    WebServer.setArgString(getComponentName(F("Slope")), toText_floatDecimals(*Slope));
    WebServer.setArgString(getComponentName(F("Intercept")), toText_floatDecimals(*Intercept));
  }
}

void PHSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("PH")), getPHText(false));
  }
}

void PHSensor_Web::websiteEvent_Button(char *Button)
{ ///When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("ReadRaw")) == 0)
    {
      updatePH(true);
    }
  }
}

void PHSensor_Web::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Slope")) == 0)
    {
      setSlope(WebServer.getArgFloat());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Intercept")) == 0)
    {
      setIntercept(WebServer.getArgFloat());
    }
  }
}