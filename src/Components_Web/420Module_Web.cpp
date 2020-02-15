#include "420Module_Web.h"
#include "Sound_Web.h"

Module_Web::Module_Web() : Module()
{
  logToSerials(F("Module_Web object created"), true, 0);
}

void Module_Web::runAll()
{
  wdt_reset();
  runSec();
  wdt_reset();
  runFiveSec();
  wdt_reset();
  runMinute();
  wdt_reset();
  runQuarterHour();
  wdt_reset();
}

void Module_Web::runReport()
{ //Reports component status to Serial output (Arduino and ESP)
  getFormattedTime(true);
  getFreeMemory();
  logToSerials(reportQueueItemCount,false,0);
  logToSerials(F(" web items refreshing:"),true,1);
  for (int i = 0; i < reportQueueItemCount; i++)
  {
    ReportQueue[i]->report();
  }
}

//////////////////////////////////////////////////////////////////
//Refresh queues: Refresh components inside the Module

void Module_Web::runSec()
{
  if (*Debug)
    logToSerials(F("One sec trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_Sec; i++)
  {
    RefreshQueue_Sec[i]->refresh_Sec();
  }
}

void Module_Web::runFiveSec()
{
  if (*Debug)
    logToSerials(F("Five sec trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_FiveSec; i++)
  {
    RefreshQueue_FiveSec[i]->refresh_FiveSec();
  }
}

void Module_Web::runMinute()
{
  if (*Debug)
    logToSerials(F("Minute trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_Minute; i++)
  {
    RefreshQueue_Minute[i]->refresh_Minute();
  }
}

void Module_Web::runQuarterHour()
{
  if (*Debug)
    logToSerials(F("Quarter hour trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_QuarterHour; i++)
  {
    logToSerials(i, true, 0);
    RefreshQueue_QuarterHour[i]->refresh_QuarterHour();
  }
}

//////////////////////////////////////////////////////////////////
//Queue subscriptions: When a component needs to get refreshed at certain intervals it subscribes to one or more refresh queues using these methods

void Module_Web::addToReportQueue(Common_Web *Component)
{
  if (QueueDepth > reportQueueItemCount)
    ReportQueue[reportQueueItemCount++] = Component;
  else
    logToSerials(F("Report queue overflow!"), true, 0); //Too many components are added to the queue, increase "QueueDepth" variable in Settings.h , or shift components to a different queue
}

void Module_Web::addToRefreshQueue_Sec(Common_Web *Component)
{
  if (QueueDepth > refreshQueueItemCount_Sec)
    RefreshQueue_Sec[refreshQueueItemCount_Sec++] = Component;
  else
    logToSerials(F("RefreshQueue_Sec overflow!"), true, 0);
}

void Module_Web::addToRefreshQueue_FiveSec(Common_Web *Component)
{
  if (QueueDepth > refreshQueueItemCount_FiveSec)
    RefreshQueue_FiveSec[refreshQueueItemCount_FiveSec++] = Component;
  else
    logToSerials(F("RefreshQueue_FiveSec overflow!"), true, 0);
}

void Module_Web::addToRefreshQueue_Minute(Common_Web *Component)
{
  if (QueueDepth > refreshQueueItemCount_Minute)
    RefreshQueue_Minute[refreshQueueItemCount_Minute++] = Component;
  else
    logToSerials(F("RefreshQueue_Minute overflow!"), true, 0);
}

void Module_Web::addToRefreshQueue_QuarterHour(Common_Web *Component)
{
  if (QueueDepth > refreshQueueItemCount_QuarterHour)
  {
    RefreshQueue_QuarterHour[refreshQueueItemCount_QuarterHour++] = Component;
  }
  else
    logToSerials(F("RefreshQueue_QuarterHour overflow!"), true, 0);
}

//////////////////////////////////////////////////////////////////
//Website subscriptions: When a component needs to get notified of a Website events from the ESP-link it subscribes to one or more website queues using these methods

void Module_Web::addToWebsiteQueue_Load(Common_Web *Component)
{
   logToSerials(F("Load queue:"),false, 0);
   logToSerials(WebsiteQueueItemCount_Load, true, 1);
  if (QueueDepth > WebsiteQueueItemCount_Load)
    WebsiteQueue_Load[WebsiteQueueItemCount_Load++] = Component;
  else
    logToSerials(F("WebsiteQueueItemCount_Load overflow!"), true, 0);
}

void Module_Web::addToWebsiteQueue_Refresh(Common_Web *Component)
{
   logToSerials(F("Refresh queue:"),false, 0);
   logToSerials(WebsiteQueueItemCount_Refresh, true, 1);
  if (QueueDepth > WebsiteQueueItemCount_Refresh)
    WebsiteQueue_Refresh[WebsiteQueueItemCount_Refresh++] = Component;
  else
    logToSerials(F("WebsiteQueueItemCount_Refresh overflow!"), true, 0);
}

void Module_Web::addToWebsiteQueue_Button(Common_Web *Component)
{
  if (QueueDepth > WebsiteQueueItemCount_Button)
    WebsiteQueue_Button[WebsiteQueueItemCount_Button++] = Component;
  else
    logToSerials(F("WebsiteQueueItemCount_Button overflow!"), true, 0);
}

void Module_Web::addToWebsiteQueue_Field(Common_Web *Component)
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
  if (*Debug)
    logToSerials(&button, true, 0);
  for (int i = 0; i < WebsiteQueueItemCount_Button; i++)
  {
    WebsiteQueue_Button[i]->websiteEvent_Button(button);
  }
}

void Module_Web::setFieldEvent(char *field)
{ //Called when any field on the website is updated.
  if (*Debug)
    logToSerials(&field, true, 0);
  for (int i = 0; i < WebsiteQueueItemCount_Field; i++)
  {
    WebsiteQueue_Field[i]->websiteEvent_Field(field);
  }
}