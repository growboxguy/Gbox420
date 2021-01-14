#include "Fan_PWM_Web.h"

Fan_PWM_Web::Fan_PWM_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::Fan_PWMSettings *DefaultSettings) : Common(Name), Fan_PWM(Name, Parent, DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToWebsiteQueue_Button(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Field(this);
}

void Fan_PWM_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"Spd\":\""));
  strcat(LongMessage, getSpeed());
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void Fan_PWM_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgInt(getComponentName(F("S")), *Speed);            ///< On hour
  }
}

void Fan_PWM_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("C")), getSpeedText(true,true));
  }
}

void Fan_PWM_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Of")) == 0)
    {
      turnOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("On")) == 0)
    {
      turnOn();
    }    
  }
}

void Fan_PWM_Web::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("S")) == 0)
    {
      setSpeed(WebServer.getArgInt());
    }
  }
}