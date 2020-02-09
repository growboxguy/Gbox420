#include "Lights_Web.h"

Lights_Web::Lights_Web(const __FlashStringHelper *Name, Module *Parent, Settings::LightsSettings *DefaultSettings) : Lights(Name,Parent,DefaultSettings)
{
  this->Parent = Parent;  
  this->Parent->AddToWebsiteQueue_Load(this);    //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  this->Parent->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  this->Parent->AddToWebsiteQueue_Field(this);   //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  this->Parent->AddToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  logToSerials(F("Lights object created"), true, 1);
}

void Lights_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgInt(getComponentName(F("OnHour")), *OnHour);
    WebServer.setArgInt(getComponentName(F("OnMinute")), *OnMinute);
    WebServer.setArgInt(getComponentName(F("OffHour")), *OffHour);
    WebServer.setArgInt(getComponentName(F("OffMinute")), *OffMinute);
    WebServer.setArgInt(getComponentName(F("Brightness")), *Brightness);
    WebServer.setArgInt(getComponentName(F("BrightnessSlider")), *Brightness);
  }
}

void Lights_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getComponentName(F("Status")), getStatusText(true));
    WebServer.setArgString(getComponentName(F("TimerEnabled")), getTimerOnOffText(true));
    WebServer.setArgString(getComponentName(F("OnTime")), getOnTimeText());
    WebServer.setArgString(getComponentName(F("OffTime")), getOffTimeText());
  }
}

void Lights_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("On")) == 0)
    {
      setLightOnOff(true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Off")) == 0)
    {
      setLightOnOff(false, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("TimerEnable")) == 0)
    {
      setTimerOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("TimerDisable")) == 0)
    {
      setTimerOnOff(false);
    }
  }
}

void Lights_Web::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Brightness")) == 0)
    {
      setBrightness(WebServer.getArgInt(), true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnHour")) == 0)
    {
      setOnHour(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnMinute")) == 0)
    {
      setOnMinute(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OffHour")) == 0)
    {
      setOffHour(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OffMinute")) == 0)
    {
      setOffMinute(WebServer.getArgInt());
    }
  }
}