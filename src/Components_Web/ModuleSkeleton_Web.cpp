#include "ModuleSkeleton_Web.h"

ModuleSkeleton_Web::ModuleSkeleton_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::ModuleSkeletonSettings *DefaultSettings) : Common(Name), ModuleSkeleton(Name, Parent, DefaultSettings), Common_Web(Name)
{ ///< constructor
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void ModuleSkeleton_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{ ///< When opening the website
  if (strcmp(url, "/Test.html.json") == 0)
  {

    WebServer.setArgBoolean(getName(F("SetPersBool"), true), *PersistentBool);
    WebServer.setArgInt(getName(F("SetPersInt"), true), *PersistentInt);
    WebServer.setArgFloat(getName(F("SetPersFloat"), true), *PersistentFloat);
    WebServer.setArgBoolean(getName(F("SetRunBool"), true), RuntimeBool);
  }
}

void ModuleSkeleton_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{ ///< When refreshing the website (Automatic, every 5sec)
  if (strcmp(url, "/Test.html.json") == 0)
  {
    WebServer.setArgBoolean(getName(F("PersistentBool"), true), *PersistentBool);
    WebServer.setArgInt(getName(F("PersistentInt"), true), *PersistentInt);
    WebServer.setArgFloat(getName(F("PersistentFloat"), true), *PersistentFloat);
    WebServer.setArgBoolean(getName(F("RuntimeBool"), true), RuntimeBool);
    WebServer.setArgInt(getName(F("RuntimeInt"), true), RuntimeInt);
    WebServer.setArgFloat(getName(F("RuntimeFloat"), true), RuntimeFloat);
    WebServer.setArgString(getName(F("RuntimeString"), true), RuntimeString);
    WebServer.setArgBoolean(getName(F("RuntimeBool"), true), RuntimeBool);
    WebServer.setArgInt(getName(F("RollingInt"), true), RollingInt->getInt(true));
    WebServer.setArgFloat(getName(F("RollingFloat"), true), RollingFloat->getFloat(true));
  }
}

bool ModuleSkeleton_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);n)
{ ///< When a button is pressed on the website
  if (!isThisMine(Command))
  {
    return false;
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
    } ///< Signals to reset average counter at next reading
    else if (strcmp_P(ShortMessage, (PGM_P)F("ResetRollFloat")) == 0)
    {
      RollingFloat->resetAverage();
    } ///< Signals to reset average counter at next reading
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetRunBool")) == 0)
    {
      RuntimeBool = WebServer.getArgBoolean();
    } ///< Getting a bool
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetRunString")) == 0)
    {
      strncpy(RuntimeString, WebServer.getArgString(), MaxWordLength);
    } ///< Getting a string
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetPersBool")) == 0)
    {
      *PersistentBool = WebServer.getArgBoolean();
    } ///< Getting a bool
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetPersInt")) == 0)
    {
      *PersistentInt = WebServer.getArgInt();
    } ///< Getting a integer
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetPersFloat")) == 0)
    {
      *PersistentFloat = WebServer.getArgFloat();
    } ///< Getting an float
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetRunInt")) == 0)
    {
      RollingInt->updateAverage((int)WebServer.getArgInt());
    } ///< Getting a integer and updating RollingAverage. Casting int32_t to int
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetRunFloat")) == 0)
    {
      RollingFloat->updateAverage(WebServer.getArgFloat());
    } ///< Getting an float
    return true;
  }
}

void ModuleSkeleton_Web::websiteEvent_Field(char *Field)
{ ///< When a field is submitted on the website
  if (!isThisMine(Field))
  {
    return;
  }
  else
  {
  }
}