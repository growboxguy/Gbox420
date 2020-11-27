#include "ModuleSkeleton_Web.h"

ModuleSkeleton_Web::ModuleSkeleton_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::ModuleSkeletonSettings *DefaultSettings) : Common(Name), ModuleSkeleton(Name, Parent, DefaultSettings), Common_Web(Name)
{ ///constructor
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToRefreshQueue_QuarterHour(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToWebsiteQueue_Button(this);
  Parent->addToWebsiteQueue_Field(this);
}

void ModuleSkeleton_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///Adds a curly bracket {  that needs to be closed at the end

  strcat_P(LongMessage, (PGM_P)F("\"}")); ///closing the curly bracket
}

void ModuleSkeleton_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{ ///When opening the website
  if (strcmp(url, "/Test.html.json") == 0)
  {

    WebServer.setArgBoolean(getComponentName(F("SetPersBool")), *PersistentBool);
    WebServer.setArgInt(getComponentName(F("SetPersInt")), *PersistentInt);
    WebServer.setArgFloat(getComponentName(F("SetPersFloat")), *PersistentFloat);
    WebServer.setArgBoolean(getComponentName(F("SetRunBool")), RuntimeBool);
  }
}

void ModuleSkeleton_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{ ///When refreshing the website (Automatic, every 5sec)
  if (strcmp(url, "/Test.html.json") == 0)
  {
    WebServer.setArgBoolean(getComponentName(F("PersistentBool")), *PersistentBool);
    WebServer.setArgInt(getComponentName(F("PersistentInt")), *PersistentInt);
    WebServer.setArgFloat(getComponentName(F("PersistentFloat")), *PersistentFloat);
    WebServer.setArgBoolean(getComponentName(F("RuntimeBool")), RuntimeBool);
    WebServer.setArgInt(getComponentName(F("RuntimeInt")), RuntimeInt);
    WebServer.setArgFloat(getComponentName(F("RuntimeFloat")), RuntimeFloat);
    WebServer.setArgString(getComponentName(F("RuntimeString")), RuntimeString);
    WebServer.setArgBoolean(getComponentName(F("RuntimeBool")), RuntimeBool);
    WebServer.setArgInt(getComponentName(F("RollingInt")), RollingInt->getInt(true));
    WebServer.setArgFloat(getComponentName(F("RollingFloat")), RollingFloat->getFloat(true));
  }
}

void ModuleSkeleton_Web::websiteEvent_Button(char *Button)
{ ///When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("ResetPers")) == 0)
    {
      *PersistentBool = false;
      *PersistentInt = 420;
      *PersistentFloat = 4.2;
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("ResetRun")) == 0)
    {
      RuntimeBool = false;
      RuntimeInt = 420;
      RuntimeFloat = 4.2;
      strncpy(RuntimeString, "HailMary", MaxWordLength);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("ResetRollInt")) == 0)
    {
      RollingInt->resetAverage();
    } ///Signals to reset average counter at next reading
    else if (strcmp_P(ShortMessage, (PGM_P)F("ResetRollFloat")) == 0)
    {
      RollingFloat->resetAverage();
    } ///Signals to reset average counter at next reading
  }
}

void ModuleSkeleton_Web::websiteEvent_Field(char *Field)
{ ///When a field is submitted on the website
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("SetRunBool")) == 0)
    {
      RuntimeBool = WebServer.getArgBoolean();
    } ///Getting a bool
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetRunString")) == 0)
    {
      strncpy(RuntimeString, WebServer.getArgString(), MaxWordLength);
    } ///Getting a string
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetPersBool")) == 0)
    {
      *PersistentBool = WebServer.getArgBoolean();
    } ///Getting a bool
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetPersInt")) == 0)
    {
      *PersistentInt = WebServer.getArgInt();
    } ///Getting a integer
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetPersFloat")) == 0)
    {
      *PersistentFloat = WebServer.getArgFloat();
    } ///Getting an float
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetRunInt")) == 0)
    {
      RollingInt->updateAverage((int)WebServer.getArgInt());
    } ///Getting a integer and updating RollingAverage. Casting int32_t to int
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetRunFloat")) == 0)
    {
      RollingFloat->updateAverage(WebServer.getArgFloat());
    } ///Getting an float
  }
}