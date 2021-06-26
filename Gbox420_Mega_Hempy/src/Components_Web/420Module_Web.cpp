#include "420Module_Web.h"
//#include "Sound_Web.h"

static char Logs[LogDepth][MaxWordLength]; ///< two dimensional array for storing log histroy displayed on the website (array of char arrays)

Module_Web::Module_Web(const __FlashStringHelper *Name) : Module(Name)
{
  //logToSerials(F("Module_Web object created"), true, 3);
}

///< Website subscriptions: If a component needs to get notified of a ESP-link Website event, it can subscribes to one or more website queues:

void Module_Web::addToWebsiteQueue_Load(Common_Web *Component)
{
  if (QueueDepth > WebsiteQueue_Load_Count)
    WebsiteQueue_Load[WebsiteQueue_Load_Count++] = Component;
  else
    logToSerials(F("WebsiteQueue_Load overflow!"), true, 0);
}

void Module_Web::addToWebsiteQueue_Refresh(Common_Web *Component)
{
  if (QueueDepth > WebsiteQueue_Refresh_Count)
    WebsiteQueue_Refresh[WebsiteQueue_Refresh_Count++] = Component;
  else
    logToSerials(F("WebsiteQueue_Refresh overflow!"), true, 0);
}

void Module_Web::addToWebsiteQueue_Button(Common_Web *Component)
{
  if (QueueDepth > WebsiteQueue_Button_Count)
    WebsiteQueue_Button[WebsiteQueue_Button_Count++] = Component;
  else
    logToSerials(F("WebsiteQueue_Button overflow!"), true, 0);
}

void Module_Web::addToWebsiteQueue_Field(Common_Web *Component)
{
  if (QueueDepth > WebsiteQueue_Field_Count)
    WebsiteQueue_Field[WebsiteQueue_Field_Count++] = Component;
  else
    logToSerials(F("WebsiteQueue_Field overflow!"), true, 0);
}

///< Website queues: Notify components in the Module_Web of a website event

void Module_Web::loadEvent(char *url)
{ ///< called when website is loaded. Runs through all components that subscribed for this event
  for (int i = 0; i < WebsiteQueue_Load_Count; i++)
  {
    WebsiteQueue_Load[i]->websiteEvent_Load(url);
  }
}

void Module_Web::refreshEvent(char *url)
{ ///< called when website is refreshed.
  for (int i = 0; i < WebsiteQueue_Refresh_Count; i++)
  {
    WebsiteQueue_Refresh[i]->websiteEvent_Refresh(url);
  }
}

void Module_Web::buttonEvent(char *button)
{ ///< Called when any button on the website is pressed.
  if (*Debug)
    logToSerials(&button, true, 0);
  for (int i = 0; i < WebsiteQueue_Button_Count; i++)
  {
    WebsiteQueue_Button[i]->websiteEvent_Button(button);
  }
}

void Module_Web::setFieldEvent(char *field)
{ ///< Called when any field on the website is updated.
  if (*Debug)
    logToSerials(&field, true, 0);
  for (int i = 0; i < WebsiteQueue_Field_Count; i++)
  {
    WebsiteQueue_Field[i]->websiteEvent_Field(field);
  }
}