#include "420Module.h"
#include "Sound.h"

Module::Module(const __FlashStringHelper *Name):Common(Name)
{
}

/* Module::Module(const __FlashStringHelper *Name, Sound * SoundFeedback) : Common(Name)
{ ///< Constructor
  this -> SoundFeedback = SoundFeedback;
  logToSerials(F("Module ready"), true, 3);
} */

void Module::runAll()
{
  wdt_reset();
  runSec();
  wdt_reset();
  runFiveSec();
  wdt_reset();
  runMinute();
  wdt_reset();
}

/**
* @brief Reports sensor readings to he Serial output (Arduino and ESP)
* @param[JSONFormat] false: Text format, true: JSON format
*/
void Module::runReport(bool JSONFormat, bool Append)
{
  if (JSONFormat)
  {
    if (!Append)
    {
      memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    }
    strcat_P(LongMessage, (PGM_P)F("{\"Log\":{")); ///< Adds two curly brackets that needs to be closed at the end
    for (int i = 0; i < reportQueueItemCount;)
    {
      ReportQueue[i++]->report(JSONFormat);
      if (i != reportQueueItemCount)
        strcat_P(LongMessage, (PGM_P)F(",")); ///< < Unless it was the last element add a , separator
    }
    strcat_P(LongMessage, (PGM_P)F("}}")); ///< closing both curly bracket
    logToSerials(&LongMessage, true, 1);
  }
  else
  {
    getFormattedTime(true);
    getFreeMemory();
    logToSerials(reportQueueItemCount, false, 2);  ///< Prints the number of items that will report
    logToSerials(F("reporting:"), true, 1);
    for (int i = 0; i < reportQueueItemCount; i++)
    {
      ReportQueue[i]->report(JSONFormat);
    }
  }
}

///< Refresh queues: Refresh components inside the Module

void Module::runSec()
{
  if (RunAllRequested)
  {
    RunAllRequested = false;
    runAll();
  }
  else
  {
    if (*Debug)
    {
      logToSerials(F("1sec trigger"), true, 1);
    }
    for (int i = 0; i < refreshQueueItemCount_Sec; i++)
    {
      RefreshQueue_Sec[i]->refresh_Sec();
    }
  }
}

void Module::runFiveSec()
{
  if (*Debug)
    logToSerials(F("5sec trigger"), true, 1);
  for (int i = 0; i < refreshQueueItemCount_FiveSec; i++)
  {
    RefreshQueue_FiveSec[i]->refresh_FiveSec();
  }
}

void Module::runMinute()
{
  if (*Debug)
    logToSerials(F("Min trigger"), true, 1);
  for (int i = 0; i < refreshQueueItemCount_Minute; i++)
  {
    RefreshQueue_Minute[i]->refresh_Minute();
  }
}

Sound *Module::getSoundObject()
{
  return SoundFeedback;
}

///< Queue subscriptions: When a component needs to get refreshed at certain intervals it subscribes to one or more refresh queues using these methods

void Module::addToReportQueue(Common *Component)
{
  if (QueueDepth > reportQueueItemCount)
    ReportQueue[reportQueueItemCount++] = Component;
  else
    logToSerials(F("Overflow"), true, 0); ///< Too many components are added to the queue, increase "QueueDepth" variable in Settings.h , or shift components to a different queue
}

void Module::addToRefreshQueue_Sec(Common *Component)
{
  if (QueueDepth > refreshQueueItemCount_Sec)
    RefreshQueue_Sec[refreshQueueItemCount_Sec++] = Component;
  else
    logToSerials(F("Overflow"), true, 0);
}

void Module::addToRefreshQueue_FiveSec(Common *Component)
{
  if (QueueDepth > refreshQueueItemCount_FiveSec)
    RefreshQueue_FiveSec[refreshQueueItemCount_FiveSec++] = Component;
  else
    logToSerials(F("Overflow"), true, 0);
}

void Module::addToRefreshQueue_Minute(Common *Component)
{
  if (QueueDepth > refreshQueueItemCount_Minute)
    RefreshQueue_Minute[refreshQueueItemCount_Minute++] = Component;
  else
    logToSerials(F("Overflow"), true, 0);
}

///< Even logs to the Serial output
void Module::addToLog(const char *LongMessage, uint8_t Indent)
{
  logToSerials(&LongMessage, true, Indent);
}

void Module::addToLog(const __FlashStringHelper *LongMessage, uint8_t Indent)
{ ///< function overloading: same function name, different parameter type
  logToSerials(&LongMessage, true, Indent);
}

///< Time

char *Module::getFormattedTime(bool PrintToSerials)
{
  time_t Now = now();                                                                                                                                // Get the current time and date from the TimeLib library
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d", year(Now), month(Now), day(Now), hour(Now), minute(Now), second(Now)); // YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
  if (PrintToSerials)
    logToSerials(&CurrentTime, true, 0);
  return CurrentTime;
}

///< Settings
void Module::setDebug(bool DebugEnabled)
{
  if (DebugEnabled != *Debug)
  {
    *Debug = DebugEnabled;
    if (*Debug)
    {
      addToLog(F("Debug ON"));
      getSoundObject()->playOnSound();
    }
    else
    {
      addToLog(F("Debug OFF"));
      getSoundObject()->playOffSound();
    }
  }
}

void Module::setMetric(bool MetricEnabled)
{
  if (MetricEnabled != *Metric)
  { //if there was a change
    *Metric = MetricEnabled;
    RunAllRequested = true; ///< Force a full sensor reading refresh
    if (*Metric)
      addToLog(F("Metric"));
    else
      addToLog(F("Imperial"));
    getSoundObject()->playOnSound();
  }
}