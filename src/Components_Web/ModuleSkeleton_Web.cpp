#include "ModuleSkeleton_Web.h"

ModuleSkeleton_Web::ModuleSkeleton_Web(const __FlashStringHelper *Name, Module *Parent, Settings::ModuleSkeletonSettings *DefaultSettings) : Common(Name)
{ //constructor
  this->Parent = Parent;
  PersistentBool = &DefaultSettings->PersistentBool;
  PersistentInt = &DefaultSettings->PersistentInt;
  PersistentFloat = &DefaultSettings->PersistentFloat;
  RollingInt = new RollingAverage();
  RollingFloat = new RollingAverage();
  Parent->addToReportQueue(this);              //Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Sec(this);         //Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method
  Parent->addToRefreshQueue_FiveSec(this);     //Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  Parent->addToRefreshQueue_Minute(this);      //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  Parent->addToRefreshQueue_QuarterHour(this); //Subscribing to the 15 minute refresh queue: Calls the refresh_QuarterHour() method
  Parent->AddToWebsiteQueue_Load(this);        //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->AddToWebsiteQueue_Refresh(this);     //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->AddToWebsiteQueue_Button(this);      //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  Parent->AddToWebsiteQueue_Field(this);       //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  logToSerials(F("ModuleSkeleton_Web object created"), true, 1);
}

void ModuleSkeleton_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{ //When opening the website
  if (strcmp(url, "/Test.html.json") == 0)
  {

    WebServer.setArgBoolean(getWebsiteComponentName(F("SetPersBool")), *PersistentBool);
    WebServer.setArgInt(getWebsiteComponentName(F("SetPersInt")), *PersistentInt);
    WebServer.setArgFloat(getWebsiteComponentName(F("SetPersFloat")), *PersistentFloat);
    WebServer.setArgBoolean(getWebsiteComponentName(F("SetRunBool")), RuntimeBool);
  }
}

void ModuleSkeleton_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{ //When refreshing the website (Automatic, every 5sec)
  if (strcmp(url, "/Test.html.json") == 0)
  {
    WebServer.setArgBoolean(getWebsiteComponentName(F("PersistentBool")), *PersistentBool);
    WebServer.setArgInt(getWebsiteComponentName(F("PersistentInt")), *PersistentInt);
    WebServer.setArgFloat(getWebsiteComponentName(F("PersistentFloat")), *PersistentFloat);
    WebServer.setArgBoolean(getWebsiteComponentName(F("RuntimeBool")), RuntimeBool);
    WebServer.setArgInt(getWebsiteComponentName(F("RuntimeInt")), RuntimeInt);
    WebServer.setArgFloat(getWebsiteComponentName(F("RuntimeFloat")), RuntimeFloat);
    WebServer.setArgString(getWebsiteComponentName(F("RuntimeString")), RuntimeString);
    WebServer.setArgBoolean(getWebsiteComponentName(F("RuntimeBool")), RuntimeBool);
    WebServer.setArgInt(getWebsiteComponentName(F("RollingInt")), RollingInt->getInt(true));
    WebServer.setArgFloat(getWebsiteComponentName(F("RollingFloat")), RollingFloat->getFloat(true));
  }
}

void ModuleSkeleton_Web::websiteEvent_Button(char *Button)
{ //When a button is pressed on the website
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
      strncpy(RuntimeString, "HailMary", MaxTextLength);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("ResetRollInt")) == 0)
    {
      RollingInt->resetAverage();
    } //Signals to reset average counter at next reading
    else if (strcmp_P(ShortMessage, (PGM_P)F("ResetRollFloat")) == 0)
    {
      RollingFloat->resetAverage();
    } //Signals to reset average counter at next reading
  }
}

void ModuleSkeleton_Web::websiteEvent_Field(char *Field)
{ //When a field is submitted on the website
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("SetRunBool")) == 0)
    {
      RuntimeBool = WebServer.getArgBoolean();
    } //Getting a bool
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetRunString")) == 0)
    {
      strncpy(RuntimeString, WebServer.getArgString(), MaxTextLength);
    } //Getting a string
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetPersBool")) == 0)
    {
      *PersistentBool = WebServer.getArgBoolean();
    } //Getting a bool
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetPersInt")) == 0)
    {
      *PersistentInt = WebServer.getArgInt();
    } //Getting a integer
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetPersFloat")) == 0)
    {
      *PersistentFloat = WebServer.getArgFloat();
    } //Getting an float
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetRunInt")) == 0)
    {
      RollingInt->updateAverage((int)WebServer.getArgInt());
    } //Getting a integer and updating RollingAverage. Casting int32_t to int
    else if (strcmp_P(ShortMessage, (PGM_P)F("SetRunFloat")) == 0)
    {
      RollingFloat->updateAverage(WebServer.getArgFloat());
    } //Getting an float
  }
}