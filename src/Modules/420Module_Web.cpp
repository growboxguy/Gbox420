#include "420Module_Web.h"

Module_Web::Module_Web(const __FlashStringHelper *Name) : Module(Name)
{ //Constructor
}

//////////////////////////////////////////////////////////////////
//Website subscriptions: When a component needs to get notified of a Website events from the ESP-link it subscribes to one or more website queues using these methods

void Module_Web::AddToWebsiteQueue_Load(Common *Component)
{
  if (QueueDepth > WebsiteQueueItemCount_Load)
    WebsiteQueue_Load[WebsiteQueueItemCount_Load++] = Component;
  else
    logToSerials(F("WebsiteQueueItemCount_Load overflow!"), true, 0);
}

void Module_Web::AddToWebsiteQueue_Refresh(Common *Component)
{
  if (QueueDepth > WebsiteQueueItemCount_Refresh)
    WebsiteQueue_Refresh[WebsiteQueueItemCount_Refresh++] = Component;
  else
    logToSerials(F("WebsiteQueueItemCount_Refresh overflow!"), true, 0);
}

void Module_Web::AddToWebsiteQueue_Button(Common *Component)
{
  if (QueueDepth > WebsiteQueueItemCount_Button)
    WebsiteQueue_Button[WebsiteQueueItemCount_Button++] = Component;
  else
    logToSerials(F("WebsiteQueueItemCount_Button overflow!"), true, 0);
}

void Module_Web::AddToWebsiteQueue_Field(Common *Component)
{
  if (QueueDepth > WebsiteQueueItemCount_Field)
    WebsiteQueue_Field[WebsiteQueueItemCount_Field++] = Component;
  else
    logToSerials(F("WebsiteQueueItemCount_Field overflow!"), true, 0);
}


//////////////////////////////////////////////////////////////////
//Website queues: Notify components in the Module_Web of a website event

void Module_Web::loadEvent(char *url)
{ //called when website is loaded. Runs through all components that subscribed for this event
  for (int i = 0; i < WebsiteQueueItemCount_Load; i++)
  {
    WebsiteQueue_Load[i]->websiteEvent_Load(url);
  }
}

void Module_Web::refreshEvent(char *url)
{ //called when website is refreshed.
  for (int i = 0; i < WebsiteQueueItemCount_Refresh; i++)
  {
    WebsiteQueue_Refresh[i]->websiteEvent_Refresh(url);
  }
}

void Module_Web::buttonEvent(char *button)
{ //Called when any button on the website is pressed.
  if (*DebugEnabled)
    logToSerials(&button, true, 0);
  for (int i = 0; i < WebsiteQueueItemCount_Button; i++)
  {
    WebsiteQueue_Button[i]->websiteEvent_Button(button);
  }
}

void Module_Web::setFieldEvent(char *field)
{ //Called when any field on the website is updated.
  if (*DebugEnabled)
    logToSerials(&field, true, 0);
  for (int i = 0; i < WebsiteQueueItemCount_Field; i++)
  {
    WebsiteQueue_Field[i]->websiteEvent_Field(field);
  }
}