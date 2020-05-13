#include "420Module.h"
#include "Sound.h"

Module::Module()
{
   logToSerials(F("Module object created"), true, 0);
}


/* Module::Module(const __FlashStringHelper *Name, Sound * SoundFeedback) : Common(Name)
{ ///Constructor
  this -> SoundFeedback = SoundFeedback;
  logToSerials(F("Module object created"), true, 0);
} */


void Module::runAll(bool AddToLog)
{
  if(AddToLog){
    logToSerials(F("Refresing all sensor readings..."), true, 0);
  }    
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

void Module::runReport(bool AddToLog)
{ ///Reports component status to Serial output (Arduino and ESP)
  getFormattedTime(true);
  getFreeMemory();  
  if(AddToLog){
    logToSerials(reportQueueItemCount,false,2);
    logToSerials(F("components reporting:"),true,1);
  }
  for (int i = 0; i < reportQueueItemCount; i++)
  {
    ReportQueue[i]->report();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Refresh queues: Refresh components inside the Module

void Module::runSec(bool AddToLog)
{
  if(RunAllRequested)
  {
    RunAllRequested = false;
    if(AddToLog){
      logToSerials(F("Running full refresh.."), true, 1);
    }
    runAll(AddToLog);
  }
  else
  {
    if (*Debug && AddToLog){
      logToSerials(F("One sec trigger.."), true, 1);
    }
    for (int i = 0; i < refreshQueueItemCount_Sec; i++)
    {
      RefreshQueue_Sec[i]->refresh_Sec();
    }
  }
}

void Module::runFiveSec(bool AddToLog)
{
  if (*Debug && AddToLog)
    logToSerials(F("Five sec trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_FiveSec; i++)
  {
    RefreshQueue_FiveSec[i]->refresh_FiveSec();
  }
}

void Module::runMinute(bool AddToLog)
{
  if (*Debug && AddToLog)
    logToSerials(F("Minute trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_Minute; i++)
  {
    RefreshQueue_Minute[i]->refresh_Minute();
  }
}

void Module::runQuarterHour(bool AddToLog)
{
  if (*Debug && AddToLog)
    logToSerials(F("Quarter hour trigger.."), true, 1);
  for (int i = 0; i < refreshQueueItemCount_QuarterHour; i++)
  {
    RefreshQueue_QuarterHour[i]->refresh_QuarterHour();
  }
}

Sound* Module::getSoundObject(){
  return SoundFeedback;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Queue subscriptions: When a component needs to get refreshed at certain intervals it subscribes to one or more refresh queues using these methods

void Module::addToReportQueue(Common *Component)
{
  if (QueueDepth > reportQueueItemCount)
    ReportQueue[reportQueueItemCount++] = Component;
  else
    logToSerials(F("Report queue overflow!"), true, 0); ///Too many components are added to the queue, increase "QueueDepth" variable in Settings.h , or shift components to a different queue
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
  if (QueueDepth > refreshQueueItemCount_QuarterHour)
  {
    RefreshQueue_QuarterHour[refreshQueueItemCount_QuarterHour++] = Component;
  }
  else
    logToSerials(F("RefreshQueue_QuarterHour overflow!"), true, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Even logs to the Serial output
void Module::addToLog(const char *LongMessage, uint8_t Indent)
{
  logToSerials(&LongMessage, true, Indent); 
}

void Module::addToLog(const __FlashStringHelper *LongMessage, uint8_t Indent)
{ ///function overloading: same function name, different parameter type
  logToSerials(&LongMessage, true, Indent);
}

///////////////////////////////////////////////////////////////
///Time

char *Module::getFormattedTime(bool PrintToSerials)
{
  time_t Now = now();                                                                                                                                /// Get the current time and date from the TimeLib library
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d", year(Now), month(Now), day(Now), hour(Now), minute(Now), second(Now)); /// YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
  if (PrintToSerials)
    logToSerials(&CurrentTime, true, 0);
  return CurrentTime;
}

//////////////////////////////////////////////////////////////////
//Settings
void Module::setDebug(bool DebugEnabled)
{
   if (DebugEnabled != *Debug)
   {
    *Debug = DebugEnabled;
    if (*Debug)
    {
      addToLog(F("Debug enabled"));
      getSoundObject() -> playOnSound();
    }
    else
    {
      addToLog(F("Debug disabled"));
      getSoundObject() -> playOffSound();
    }
   }  
}

void Module::setMetric(bool MetricEnabled)
{
  if (MetricEnabled != *Metric)
  { //if there was a change
    *Metric = MetricEnabled;
    RunAllRequested = true;  ///< Force a full sensor reading refresh
    if (*Metric)
      addToLog(F("Using Metric units"));
    else
      addToLog(F("Using Imperial units"));
    getSoundObject() -> playOnSound();
  }
}