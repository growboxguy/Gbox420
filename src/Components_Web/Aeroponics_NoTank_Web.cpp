#include "Aeroponics_NoTank_Web.h"

Aeroponics_NoTank_Web::Aeroponics_NoTank_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) : Common(Name), Aeroponics_NoTank(Name, Parent, DefaultSettings, FeedbackPressureSensor, Pump), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
  Parent->addToWebsiteQueue_Field(this);
}

void Aeroponics_NoTank_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end

  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void Aeroponics_NoTank_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgInt(getName(F("Timeout")), Pump->getPumpTimeOut());
    WebServer.setArgInt(getName(F("Priming")), Pump->getPrimingTime());
    WebServer.setArgInt(getName(F("Int")), *Interval);
    WebServer.setArgFloat(getName(F("Dur")), *Duration);
  }
}

void Aeroponics_NoTank_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getName(F("Pres")), FeedbackPressureSensor->getPressureText(false,true));
    WebServer.setArgString(getName(F("Spray")), sprayStateToText());
    WebServer.setArgString(getName(F("Pump")), Pump->getStateText());
    WebServer.setArgString(getName(F("LastSP")), toText_pressure(LastSprayPressure));
  }
}

void Aeroponics_NoTank_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMine(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("BypassOn")) == 0)
    {
      Pump->turnBypassOn();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("BypassOff")) == 0)
    {
      Pump->turnBypassOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOn")) == 0)
    {
      Pump->startPump(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOff")) == 0)
    {
      Pump->stopPump();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpDis")) == 0)
    {
      Pump->disablePump();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Mix")) == 0)
    {
      Pump->startMixing();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayEn")) == 0)
    {
      setSprayOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayDis")) == 0)
    {
      setSprayOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayNow")) == 0)
    {
      sprayNow(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayOff")) == 0)
    {
      sprayOff();
    }
  }
}

void Aeroponics_NoTank_Web::websiteEvent_Field(__attribute__((unused)) char *Field)
{ ///< When a field is submitted using the Set button
  if (strcmp_P(ShortMessage, (PGM_P)F("Timeout")) == 0)
  {
    Pump->setPumpTimeOut(WebServer.getArgInt());
  }
  else if (strcmp_P(ShortMessage, (PGM_P)F("Priming")) == 0)
  {
    Pump->setPrimingTime(WebServer.getArgInt());
  }
  else if (strcmp_P(ShortMessage, (PGM_P)F("Dur")) == 0)
  {
    setSprayDuration(WebServer.getArgFloat());
  }
  else if (strcmp_P(ShortMessage, (PGM_P)F("Int")) == 0)
  {
    setSprayInterval(WebServer.getArgInt());
  }
  else if (strcmp_P(ShortMessage, (PGM_P)F("Spray")) == 0)
  {
    setSprayOnOff(WebServer.getArgBoolean());
  }
}