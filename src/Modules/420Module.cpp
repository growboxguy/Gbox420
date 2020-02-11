#include "420Module.h"
#include "../Components/Sound.h"

static char Logs[LogDepth][MaxTextLength]; //two dimensional array for storing log histroy displayed on the website (array of char arrays)

Module::Module()
{ //Constructor
  logToSerials(F("Module object created"), true, 0);
}

/*
Module::Module(const __FlashStringHelper *Name, Sound * SoundFeedback) : Common(Name)
{ //Constructor
  this -> SoundFeedback = SoundFeedback;
  logToSerials(F("Module object created"), true, 0);
}
*/

void Module::runAll()
{
  wdt_reset();
  runSec();
  wdt_reset();
  runFiveSec();
  wdt_reset();
  refresh_Minute();
  wdt_reset();
  runQuarterHour();
  wdt_reset();
}

void Module::runReport()
{ //Reports component status to Serial output (Arduino and ESP)
  getFormattedTime(true);
  getFreeMemory();
  for (int i = 0; i < reportQueueItemCount; i++)
  {
    ReportQueue[i]->report();
  }
}

//////////////////////////////////////////////////////////////////
//Queue subscriptions: When a component needs to get refreshed at certain intervals it subscribes to one or more refresh queues using these methods

void Module::addToReportQueue(Common *Component)
{
  if (QueueDepth > reportQueueItemCount)
    ReportQueue[reportQueueItemCount++] = Component;
  else
    logToSerials(F("Report queue overflow!"), true, 0); //Too many components are added to the queue, increase "QueueDepth" variable in Settings.h , or shift components to a different queue
}

void Module::addToRefreshQueue_Sec(Common *Component)
{
  if (QueueDepth > refreshQueueItemCount_Sec)
    RefreshQueue_Sec[refreshQueueItemCount_Sec++] = Component;
  else
    logToSerials(F("RefreshQueue_Sec overflow!"), true, 0);
}

void Module::addToRefreshQueue_FiveSec(Common *Component)
{
  if (QueueDepth > refreshQueueItemCount_FiveSec)
    RefreshQueue_FiveSec[refreshQueueItemCount_FiveSec++] = Component;
  else
    logToSerials(F("RefreshQueue_FiveSec overflow!"), true, 0);
}

void Module::addToRefreshQueue_Minute(Common *Component)
{
  if (QueueDepth > refreshQueueItemCount_Minute)
    RefreshQueue_Minute[refreshQueueItemCount_Minute++] = Component;
  else
    logToSerials(F("RefreshQueue_Minute overflow!"), true, 0);
}

void Module::addToRefreshQueue_QuarterHour(Common *Component)
{
  logToSerials(F("Quarter:"), false, 0);
  if (QueueDepth > refreshQueueItemCount_QuarterHour)
  {
    RefreshQueue_QuarterHour[refreshQueueItemCount_QuarterHour++] = Component;
  }
  else
    logToSerials(F("RefreshQueue_QuarterHour overflow!"), true, 0);
}

//////////////////////////////////////////////////////////////////
//Refresh queues: Refresh components inside the Module

void Module::runSec()
{
  if (*DebugEnabled)
    logToSerials(F("One sec trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_Sec; i++)
  {
    RefreshQueue_Sec[i]->refresh_Sec();
  }
}

void Module::runFiveSec()
{
  if (*DebugEnabled)
    logToSerials(F("Five sec trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_FiveSec; i++)
  {
    RefreshQueue_FiveSec[i]->refresh_FiveSec();
  }
}

void Module::runMinute()
{
  if (*DebugEnabled)
    logToSerials(F("Minute trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_Minute; i++)
  {
    RefreshQueue_Minute[i]->refresh_Minute();
  }
}

void Module::runQuarterHour()
{
  if (*DebugEnabled)
    logToSerials(F("Quarter hour trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_QuarterHour; i++)
  {
    logToSerials(i, true, 0);
    RefreshQueue_QuarterHour[i]->refresh_QuarterHour();
  }
}

Sound* Module::getSoundObject(){
  return SoundFeedback;
}

//////////////////////////////////////////////////////////////////
//Even logs on the website
void Module::addToLog(const char *LongMessage, byte Indent)
{ //adds a log entry that is displayed on the web interface
  logToSerials(LongMessage, true, Indent);
  for (byte i = LogDepth - 1; i > 0; i--)
  {                                       //Shift every log entry one up, dropping the oldest
    memset(&Logs[i], 0, sizeof(Logs[i])); //clear variable
    strncpy(Logs[i], Logs[i - 1], MaxTextLength);
  }
  memset(&Logs[0], 0, sizeof(Logs[0]));         //clear variable
  strncpy(Logs[0], LongMessage, MaxTextLength); //instert new log to [0]
}

void Module::addToLog(const __FlashStringHelper *LongMessage, byte Indent)
{ //function overloading: same function name, different parameter type
  logToSerials(LongMessage, true, Indent);
  for (byte i = LogDepth - 1; i > 0; i--)
  {                                       //Shift every log entry one up, dropping the oldest
    memset(&Logs[i], 0, sizeof(Logs[i])); //clear variable
    strncpy(Logs[i], Logs[i - 1], MaxTextLength);
  }
  memset(&Logs[0], 0, sizeof(Logs[0]));                  //clear variable
  strncpy_P(Logs[0], (PGM_P)LongMessage, MaxTextLength); //instert new log to [0]
}

char *Module::eventLogToJSON(bool Append)
{ //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
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
