#include "Lights_Web.h"

Lights_Web::Lights_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightsSettings *DefaultSettings) : Common(Name), Common_Web(Name), Lights(Name, Parent, DefaultSettings)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void Lights_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  WebServer.setArgInt(getName(F("OnH"), true), *OnHour);    ///< On hour
  WebServer.setArgInt(getName(F("OnM"), true), *OnMinute);  ///< On minute
  WebServer.setArgInt(getName(F("OfH"), true), *OffHour);   ///< Off hour
  WebServer.setArgInt(getName(F("OfM"), true), *OffMinute); ///< Off minute
  WebServer.setArgInt(getName(F("B"), true), *Brightness);  ///< Brightness percentage
  // WebServer.setArgBoolean(getName(F("F")), *FadingEnabled);   ///< Enable or disable Fade in/out
  // WebServer.setArgInt(getName(F("FInc")), *FadingIncrements); ///< Fade change (%)
  // WebServer.setArgInt(getName(F("FInt")), *FadingInterval);   ///< Fade step interval (sec)
  // WebServer.setArgInt(getName(F("DD"),true), *DimmingDuration); ///< Fade step interval (sec)
}

void Lights_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  WebServer.setArgString(getName(F("S"), true), getStateText());                  ///< State
  WebServer.setArgString(getName(F("Br"), true), getCurrentBrightnessText(true)); ///< Timer on or off
  WebServer.setArgString(getName(F("T"), true), getTimerOnOffText(true));         ///< Timer on or off
}

bool Lights_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("On")) == 0)
    {
      setLightOnOff(true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Of")) == 0)
    {
      setLightOnOff(false, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("TOn")) == 0)
    {
      setTimerOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("TOff")) == 0)
    {
      setTimerOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("D")) == 0)
    {
      dimLightsOnOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B")) == 0)
    {
      setBrightness(WebServer.getArgInt(), true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnH")) == 0)
    {
      setOnHour(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnM")) == 0)
    {
      setOnMinute(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OfH")) == 0)
    {
      setOffHour(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OfM")) == 0)
    {
      setOffMinute(WebServer.getArgInt());
    }
    // else if (strcmp_P(ShortMessage, (PGM_P)F("F")) == 0)
    // {
    //   setFadeOnOff(WebServer.getArgBoolean());
    // }
    // else if (strcmp_P(ShortMessage, (PGM_P)F("FInc")) == 0)
    // {
    //   setFadeIncrements(WebServer.getArgInt());
    // }
    // else if (strcmp_P(ShortMessage, (PGM_P)F("FInt")) == 0)
    // {
    //   setFadeInterval(WebServer.getArgInt());
    // }
    else if (strcmp_P(ShortMessage, (PGM_P)F("DD")) == 0)
    {
      setDimDuration(WebServer.getArgInt());
    }
    return true;
  }
}