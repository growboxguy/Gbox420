#include "420Module_Web.h"
#include "Sound_Web.h"

static char Logs[LogDepth][MaxTextLength]; ///two dimensional array for storing log histroy displayed on the website (array of char arrays)

Module_Web::Module_Web(RF24 *Wireless) : Module()
{
  this->Wireless = Wireless;
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
{ ///Reports component status to Serial output (Arduino and ESP)
  getFormattedTime(true);
  getFreeMemory();
  logToSerials(reportQueueItemCount,false,0);
  logToSerials(F("web components refreshing:"),true,1);
  for (int i = 0; i < reportQueueItemCount; i++)
  {
    ReportQueue[i]->report();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Refresh queues: Refresh components inside the Module

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Queue subscriptions: When a component needs to get refreshed at certain intervals it subscribes to one or more refresh queues using these methods

void Module_Web::addToReportQueue(Common_Web *Component)
{
  if (QueueDepth > reportQueueItemCount)
    ReportQueue[reportQueueItemCount++] = Component;
  else
    logToSerials(F("Report queue overflow!"), true, 0); ///Too many components are added to the queue, increase "QueueDepth" variable in Settings.h , or shift components to a different queue
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Website subscriptions: When a component needs to get notified of a Website events from the ESP-link it subscribes to one or more website queues using these methods

void Module_Web::addToWebsiteQueue_Load(Common_Web *Component)
{   
  if (QueueDepth > WebsiteQueueItemCount_Load)
    WebsiteQueue_Load[WebsiteQueueItemCount_Load++] = Component;
  else
    logToSerials(F("WebsiteQueueItemCount_Load overflow!"), true, 0);
}

void Module_Web::addToWebsiteQueue_Refresh(Common_Web *Component)
{   
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Website queues: Notify components in the Module_Web of a website event

void Module_Web::loadEvent(char *url)
{ ///called when website is loaded. Runs through all components that subscribed for this event
  for (int i = 0; i < WebsiteQueueItemCount_Load; i++)
  {
    WebsiteQueue_Load[i]->websiteEvent_Load(url);
  }
}

void Module_Web::refreshEvent(char *url)
{ ///called when website is refreshed.
  for (int i = 0; i < WebsiteQueueItemCount_Refresh; i++)
  {
    WebsiteQueue_Refresh[i]->websiteEvent_Refresh(url);
  }
}

void Module_Web::buttonEvent(char *button)
{ ///Called when any button on the website is pressed.
  if (*Debug)
    logToSerials(&button, true, 0);
  for (int i = 0; i < WebsiteQueueItemCount_Button; i++)
  {
    WebsiteQueue_Button[i]->websiteEvent_Button(button);
  }
}

void Module_Web::setFieldEvent(char *field)
{ ///Called when any field on the website is updated.
  if (*Debug)
    logToSerials(&field, true, 0);
  for (int i = 0; i < WebsiteQueueItemCount_Field; i++)
  {
    WebsiteQueue_Field[i]->websiteEvent_Field(field);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Even logs on the website
void Module_Web::addToLog(const char *LongMessage, __attribute__((unused)) uint8_t Indent)
{ ///adds a log entry that is displayed on the web interface
  ///logToSerials(&LongMessage, true, Indent);
  for (uint8_t i = LogDepth - 1; i > 0; i--)
  {                                       ///Shift every log entry one up, dropping the oldest
    memset(&Logs[i], 0, sizeof(Logs[i])); ///clear variable
    strncpy(Logs[i], Logs[i - 1], MaxTextLength);
  }
  memset(&Logs[0], 0, sizeof(Logs[0]));         ///clear variable
  strncpy(Logs[0], LongMessage, MaxTextLength); ///instert new log to [0]
}

void Module_Web::addToLog(const __FlashStringHelper *LongMessage, __attribute__((unused)) uint8_t Indent)
{ ///function overloading: same function name, different parameter type
  ///logToSerials(&LongMessage, true, Indent);
  for (uint8_t i = LogDepth - 1; i > 0; i--)
  {                                       ///Shift every log entry one up, dropping the oldest
    memset(&Logs[i], 0, sizeof(Logs[i])); ///clear variable
    strncpy(Logs[i], Logs[i - 1], MaxTextLength);
  }
  memset(&Logs[0], 0, sizeof(Logs[0]));                  ///clear variable
  strncpy_P(Logs[0], (PGM_P)LongMessage, MaxTextLength); ///instert new log to [0]
}

char *Module_Web::eventLogToJSON(bool Append)
{ ///Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  if (!Append)
    memset(&LongMessage[0], 0, sizeof(LongMessage));
  strcat_P(LongMessage, (PGM_P)F("["));
  for (int i = LogDepth - 1; i >= 0; i--)
  {
    strcat_P(LongMessage, (PGM_P)F("\""));
    strcat(LongMessage, Logs[i]);
    strcat_P(LongMessage, (PGM_P)F("\""));
    if (i > 0)
      strcat_P(LongMessage, (PGM_P)F(","));
  }
  LongMessage[strlen(LongMessage)] = ']';
  return LongMessage;
}

///Google Sheets functions

void Module_Web::relayToGoogleSheets(const __FlashStringHelper *Title, char (*JSONData)[MaxLongTextLength])
{
  char ValueToReport[MaxLongTextLength] = "";
  strcat_P(ValueToReport, (PGM_P)F("/pushingbox?devid="));
  strcat(ValueToReport, ModuleSettings -> PushingBoxLogRelayID);
  strcat_P(ValueToReport, (PGM_P)F("&BoxData={\""));
  strcat_P(ValueToReport, (PGM_P)Title);
  strcat_P(ValueToReport, (PGM_P)F("\":"));
  strcat(ValueToReport, *JSONData);
  strcat_P(ValueToReport, (PGM_P)F("}"));
  if (*Debug)
  { ///print the report command to console
    logToSerials(F("api.pushingbox.com"), false, 4);
    logToSerials(&ValueToReport, true, 0);
  }
  PushingBoxRestAPI.get(ValueToReport); ///PushingBoxRestAPI will append http:///api.pushingbox.com/ in front of the command
}