#include "Lights_Web.h"

Lights_Web::Lights_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightsSettings *DefaultSettings) : Lights(Name,Parent,DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToWebsiteQueue_Load(this);    //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->addToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->addToWebsiteQueue_Field(this);   //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  Parent->addToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
}

void Lights_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgInt(getComponentName(F("OnH")), *OnHour);   //On hour
    WebServer.setArgInt(getComponentName(F("OnM")), *OnMinute);   //On minute
    WebServer.setArgInt(getComponentName(F("OfH")), *OffHour);    //Off hour
    WebServer.setArgInt(getComponentName(F("OfM")), *OffMinute);  //Off minute
    WebServer.setArgInt(getComponentName(F("B")), *Brightness);  //Brightness percentage 
    WebServer.setArgInt(getComponentName(F("BS")), *Brightness); //Brightness slider 
  }
}

void Lights_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getComponentName(F("S")), getStatusText(true));
    WebServer.setArgString(getComponentName(F("T")), getTimerOnOffText(true));
    WebServer.setArgString(getComponentName(F("OnT")), getOnTimeText());
    WebServer.setArgString(getComponentName(F("OfT")), getOffTimeText());
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
    if (strcmp_P(ShortMessage, (PGM_P)F("On")) == 0){setLightOnOff(true, true);}
    else if (strcmp_P(ShortMessage, (PGM_P)F("Of")) == 0){setLightOnOff(false, true);}
    else if (strcmp_P(ShortMessage, (PGM_P)F("TOn")) == 0){setTimerOnOff(true);}
    else if (strcmp_P(ShortMessage, (PGM_P)F("TOff")) == 0){setTimerOnOff(false);}
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
    if (strcmp_P(ShortMessage, (PGM_P)F("B")) == 0){setBrightness(WebServer.getArgInt(), true);}
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnH")) == 0){setOnHour(WebServer.getArgInt());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnM")) == 0){setOnMinute(WebServer.getArgInt());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("OfH")) == 0){setOffHour(WebServer.getArgInt());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("OfM")) == 0){setOffMinute(WebServer.getArgInt());}
  }
}