#include "Lights_Web.h"

Lights_Web::Lights_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightsSettings *DefaultSettings) : Common(Name), Lights(Name,Parent,DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);     
  Parent->addToRefreshQueue_Sec(this);     
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Load(this);    
  Parent->addToWebsiteQueue_Refresh(this); 
  Parent->addToWebsiteQueue_Field(this);   
  Parent->addToWebsiteQueue_Button(this);  
}

void Lights_Web::reportToJSON()
{
    Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"Status\":\""));
    strcat(LongMessage, getStatusText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"Brightness\":\""));
    strcat(LongMessage, getBrightnessText());   
    strcat_P(LongMessage, (PGM_P)F("\",\"Timer\":\""));
    strcat(LongMessage, getTimerOnOffText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"On\":\""));
    strcat(LongMessage, getOnTimeText());
    strcat_P(LongMessage, (PGM_P)F("\",\"Off\":\""));
    strcat(LongMessage, getOffTimeText());
    strcat_P(LongMessage, (PGM_P)F("\"}"));  ///< closing the curly bracket
}

void Lights_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgInt(getComponentName(F("OnH")), *OnHour);   ///On hour
    WebServer.setArgInt(getComponentName(F("OnM")), *OnMinute);   ///On minute
    WebServer.setArgInt(getComponentName(F("OfH")), *OffHour);    ///Off hour
    WebServer.setArgInt(getComponentName(F("OfM")), *OffMinute);  ///Off minute
    WebServer.setArgInt(getComponentName(F("B")), *Brightness);  ///Brightness percentage 
    WebServer.setArgInt(getComponentName(F("BS")), *Brightness); ///Brightness slider
    WebServer.setArgBoolean(getComponentName(F("F")), *FadingEnabled); ///Enable or disable Fade in/out 
    WebServer.setArgInt(getComponentName(F("FInc")), *FadingIncrements);  ///Fade change (%)
    WebServer.setArgInt(getComponentName(F("FInt")), *FadingInterval); ///Fade step interval (sec)
  }
}

void Lights_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("S")), getStateText());  ///State
    WebServer.setArgString(getComponentName(F("Br")), getBrightnessText());  ///Timer on or off
    WebServer.setArgString(getComponentName(F("T")), getTimerOnOffText(true));  ///Timer on or off
    WebServer.setArgString(getComponentName(F("OnT")), getOnTimeText());   ///Turn on time
    WebServer.setArgString(getComponentName(F("OfT")), getOffTimeText());  ///Turn off time
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
    if (strcmp_P(ShortMessage, (PGM_P)F("B")) == 0){setBrightness(WebServer.getArgInt(), true, true);}
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnH")) == 0){setOnHour(WebServer.getArgInt());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnM")) == 0){setOnMinute(WebServer.getArgInt());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("OfH")) == 0){setOffHour(WebServer.getArgInt());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("OfM")) == 0){setOffMinute(WebServer.getArgInt());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("F")) == 0){setFadeOnOff(WebServer.getArgBoolean());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("FInc")) == 0){setFadeIncrements(WebServer.getArgInt());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("FInt")) == 0){setFadeInterval(WebServer.getArgInt());}
  }
}