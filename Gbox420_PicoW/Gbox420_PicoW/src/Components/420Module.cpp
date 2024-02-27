#include "420Module.h"
#include "Sound.h"

Module::Module(const char *Name) : Common(Name)
{ ///< Constructor
  printf("   Module ready\n");
} 

Module::Module(const char *Name, Sound * SoundFeedback) : Common(Name)
{ ///< Constructor
  this -> SoundFeedback = SoundFeedback;
  printf("   Module ready\n");
} 

/**
* @brief Run every thread
*/
void Module::runAll()
{
  //wdt_reset();
  runSec();
 // wdt_reset();
  runFiveSec();
 // wdt_reset();
  runMinute();
 // wdt_reset();
}

/**
* @brief Handles custom reporting frequency for Serial
* @param ForceRun Send a report instantly, even when regular reports are disabled
* @param ClearBuffer Flush the LongMessage buffer before starting to report
* @param KeepBuffer Stores the full JSON report in the LongMessage buffer - Only use this on the Mega2560 where LongMessage is large enough to store a complete report (Can be up to 1024kB)
* @param JSONToBufferOnly Do not print anyting on the serial output, only fll the LongMessage buffer with the JSON report
*/
void Module::reportToSerialTrigger(bool ForceRun, bool ClearBuffer, bool KeepBuffer, bool JSONToBufferOnly)
{
  if ((SerialTriggerCounter++ % (*SerialReportFrequency / 5) == 0) || ForceRun)
  {
    runReport(ForceRun, ClearBuffer, KeepBuffer, JSONToBufferOnly);
  }
}

/**
* @brief Set how often a report should be sent to the Serial output (Arduino and ESP)
* @param Frequency Send a report every X seconds
*/
void Module::setSerialReportingFrequency(uint16_t Frequency)
{
  if (Frequency != *SerialReportFrequency)
  {
    *SerialReportFrequency = Frequency;
  }
  getSoundObject()->playOnSound();
}

/**
* @brief Reports sensor readings to the Serial output (Arduino and ESP) or to the LongMessage bufer
* @param ForceRun Send a report instantly, even when regular reports are disabled
* @param ClearBuffer Flush the LongMessage buffer before starting to report
* @param KeepBuffer Stores the full JSON report in the LongMessage buffer - Only use this on the Mega2560 where LongMessage is large enough to store a complete report (Can be up to 1024kB)
* @param JSONToBufferOnly Do not print anyting on the serial output, only fll the LongMessage buffer with the JSON report
*/
void Module::runReport(bool ForceRun, bool ClearBuffer, bool KeepBuffer, bool JSONToBufferOnly)
{
  if ((*SerialReportDate || ForceRun) && !JSONToBufferOnly)
  {
    getFormattedTime(true);
  }
  if ((*SerialReportMemory || ForceRun) && !JSONToBufferOnly)
  {
    //getFreeMemory();
  }
  /*
  if ((*SerialReportJSONFriendly  || ForceRun) && !JSONToBufferOnly)
  {
    printf(reportQueueItemCount); ///< Prints the number of items that will report
    printf("reporting:");
    for (int i = 0; i < reportQueueItemCount; i++)
    {
      ReportQueue[i]->report(false);
    }
  }
  */
  if (*SerialReportJSON || ForceRun || JSONToBufferOnly)
  {
    if (ClearBuffer)
    {
      memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    }
    strcat(LongMessage, "{\"Log\":{"); ///< Adds two curly brackets that needs to be closed at the end
    if (!KeepBuffer)
    {
      printf("%s\n",&LongMessage);
      memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    }
    for (int i = 0; i < reportQueueItemCount;)
    {
      ReportQueue[i++]->report(JSONToBufferOnly || KeepBuffer ? false : *SerialReportJSONFriendly);
      if (i != reportQueueItemCount)
        strcat(LongMessage, ","); ///< < Unless it was the last element add a , separator
      if (!KeepBuffer)
      {
        printf("%s\n",&LongMessage);
        memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
      }
    }
    strcat(LongMessage, "}}"); ///< closing both curly bracket
    if (!JSONToBufferOnly)
      printf("%s\n",&LongMessage);
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
      printf(" 1sec\n");
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
    printf(" 5sec\n");
  for (int i = 0; i < refreshQueueItemCount_FiveSec; i++)
  {
    RefreshQueue_FiveSec[i]->refresh_FiveSec();
  }
}

void Module::runMinute()
{
  if (*Debug)
    printf(" 1min\n");
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
    printf("Report Overflow\n"); ///< Too many components are added to the queue, increase "QueueDepth" variable in Settings.h , or shift components to a different queue
}

void Module::addToRefreshQueue_Sec(Common *Component)
{
  if (QueueDepth > refreshQueueItemCount_Sec)
    RefreshQueue_Sec[refreshQueueItemCount_Sec++] = Component;
  else
    printf("Refresh 1s Overflow\n");
}

void Module::addToRefreshQueue_FiveSec(Common *Component)
{
  if (QueueDepth > refreshQueueItemCount_FiveSec)
    RefreshQueue_FiveSec[refreshQueueItemCount_FiveSec++] = Component;
  else
    printf("Refresh 5s Overflow\n");
}

void Module::addToRefreshQueue_Minute(Common *Component)
{
  if (QueueDepth > refreshQueueItemCount_Minute)
    RefreshQueue_Minute[refreshQueueItemCount_Minute++] = Component;
  else
    printf("Refresh 1m Overflow\n");
}

///< Even logs to the Serial output
void Module::addToLog(const char *LongMessage, uint8_t Indent)
{
  printf("%s\n",&LongMessage);
}


///< Time

char *Module::getFormattedTime(bool PrintToSerials)  // TODO: Fix this
{
  /*
  time_t Now = now();                                                                                                                          // Get the current time and date from the TimeLib library
  snprintf(CurrentTime, MaxWordLength, "%04d/%02d/%02d-%02d:%02d:%02d", year(Now), month(Now), day(Now), hour(Now), minute(Now), second(Now)); // YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
  if (PrintToSerials)
    printf("%s\n",&CurrentTime);
    */
  return CurrentTime;
}

///< Settings
void Module::setDebug(bool DebugEnabled)
{
  if (DebugEnabled != *Debug)
  {
    *Debug = DebugEnabled;
  }
  getSoundObject()->playOnOffSound(*Debug);
}

void Module::setMetric(bool MetricEnabled)
{
  if (MetricEnabled != *Metric)
  { //if there was a change
    *Metric = MetricEnabled;
    RunAllRequested = true; ///< Force a full sensor reading refresh
  }
  getSoundObject()->playOnOffSound(*Metric);
}

void Module::setSerialReportDate(bool State)
{
  if (State != *SerialReportDate)
  { //if there was a change
    *SerialReportDate = State;
  }
  getSoundObject()->playOnOffSound(*SerialReportDate);
}

void Module::setSerialReportMemory(bool State)
{
  if (State != *SerialReportMemory)
  { //if there was a change
    *SerialReportMemory = State;
  }
  getSoundObject()->playOnOffSound(*SerialReportMemory);
}

void Module::setSerialReportJSONFriendly(bool State)
{
  if (State != *SerialReportJSONFriendly)
  { //if there was a change
    *SerialReportJSONFriendly = State;
  }
  getSoundObject()->playOnOffSound(*SerialReportJSONFriendly);
}

void Module::setSerialReportJSON(bool State)
{
  if (State != *SerialReportJSON)
  { //if there was a change
    *SerialReportJSON = State;
  }
  getSoundObject()->playOnOffSound(*SerialReportJSON);
}

void Module::setSerialReportWireless(bool State)
{
  if (State != *SerialReportWireless)
  { //if there was a change
    *SerialReportWireless = State;
  }
  getSoundObject()->playOnOffSound(*SerialReportWireless);
}