#include "Lights_Web.h"
#include "GrowBox.h"

Lights_Web::Lights_Web(const __FlashStringHelper *Name, Module *Parent, Settings::LightsSettings *DefaultSettings) : Lights(Name,Parent,DefaultSettings)
{
  this->Parent = Parent;  
  Parent->AddToWebsiteQueue_Load(this);    //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->AddToWebsiteQueue_Field(this);   //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  Parent->AddToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  logToSerials(F("Lights object created"), true, 1);
}

void Lights_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgInt(getWebsiteComponentName(F("OnHour")), *OnHour);
    WebServer.setArgInt(getWebsiteComponentName(F("OnMinute")), *OnMinute);
    WebServer.setArgInt(getWebsiteComponentName(F("OffHour")), *OffHour);
    WebServer.setArgInt(getWebsiteComponentName(F("OffMinute")), *OffMinute);
    WebServer.setArgInt(getWebsiteComponentName(F("Brightness")), *Brightness);
    WebServer.setArgInt(getWebsiteComponentName(F("BrightnessSlider")), *Brightness);
  }
}

void Lights_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Status")), getStatusText(true));
    WebServer.setArgString(getWebsiteComponentName(F("TimerEnabled")), getTimerOnOffText(true));
    WebServer.setArgString(getWebsiteComponentName(F("OnTime")), getOnTimeText());
    WebServer.setArgString(getWebsiteComponentName(F("OffTime")), getOffTimeText());
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