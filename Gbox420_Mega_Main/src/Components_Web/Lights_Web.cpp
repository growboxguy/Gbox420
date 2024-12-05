#include "Lights_Web.h"

Lights_Web::Lights_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightsSettings *DefaultSettings) : Common(Name), Common_Web(Name), Lights(Name, Parent, DefaultSettings), Parent(Parent)
{
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void Lights_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  WebServer.setArgInt(getName(F("OnH"), true), OnHour);    ///< On hour
  WebServer.setArgInt(getName(F("OnM"), true), OnMinute);  ///< On minute
  WebServer.setArgInt(getName(F("OfH"), true), OffHour);   ///< Off hour
  WebServer.setArgInt(getName(F("OfM"), true), OffMinute); ///< Off minute
  WebServer.setArgInt(getName(F("B"), true), Brightness);  ///< Brightness percentage
  // WebServer.setArgBoolean(getName(F("F")), FadingEnabled);   ///< Enable or disable Fade in/out
  // WebServer.setArgInt(getName(F("FInc")), FadingIncrements); ///< Fade change (%)
  // WebServer.setArgInt(getName(F("FInt")), FadingInterval);   ///< Fade step interval (sec)
  // WebServer.setArgInt(getName(F("DD"),true), DimmingDuration); ///< Fade step interval (sec)
}

void Lights_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
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
    else if (strcmp_P(ShortMessage, (PGM_P)F("S")) == 0)
    {
      setLightOnOff(toBool(Data), true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("TOn")) == 0)
    {
      setTimerOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("TOf")) == 0)
    {
      setTimerOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("T")) == 0)
    {
      setTimerOnOff(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("D")) == 0)
    {
      dimLightsOnOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B")) == 0)
    {
      setBrightness(toInt(Data), true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnH")) == 0)
    {
      setOnHour(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnM")) == 0)
    {
      setOnMinute(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OnT")) == 0)
    {
      setOnTime(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OfH")) == 0)
    {
      setOffHour(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OfM")) == 0)
    {
      setOffMinute(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OfT")) == 0)
    {
      setOffTime(Data);
    }
    // else if (strcmp_P(ShortMessage, (PGM_P)F("F")) == 0)
    // {
    //   setFadeOnOff(toBool(Data));
    // }
    // else if (strcmp_P(ShortMessage, (PGM_P)F("FInc")) == 0)
    // {
    //   setFadeIncrements(toInt(Data);
    // }
    // else if (strcmp_P(ShortMessage, (PGM_P)F("FInt")) == 0)
    // {
    //   setFadeInterval(toInt(Data);
    // }
    else if (strcmp_P(ShortMessage, (PGM_P)F("DD")) == 0)
    {
      setDimDuration(toInt(Data));
    }
    return true;
  }
}