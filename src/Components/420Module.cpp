#include "420Module.h"
#include "Sound.h"

Module::Module(const __FlashStringHelper *Name) : Common(Name)
{
  //logToSerials(F("Module object created"), true, 3);
}

/* Module::Module(const __FlashStringHelper *Name, Sound * SoundFeedback) : Common(Name)
{ ///< Constructor
  this -> SoundFeedback = SoundFeedback;
  logToSerials(F("Module ready"), true, 3);
} */

/**
* @brief Run every thread
*/
void Module::runAll()
{
  wdt_reset();
  run1sec();
  wdt_reset();
  run5sec();
  wdt_reset();
  run1min();
  wdt_reset();
}

/**
* @brief Handles custom reporting frequency for Serial
* @param ForceRun Send a report instantly, even when regular reports are disabled
* @param ClearBuffer Flush the LongMessage buffer before starting to report
* @param KeepBuffer Stores the full JSON report in the LongMessage buffer - Only use this on the Mega2560 where LongMessage is large enough to store a complete report (Can be up to 1024kB)
* @param JSONToBufferOnly Do not print anything on the serial output, only fill the LongMessage buffer with the JSON report
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
* @brief Reports sensor readings to the Serial output (Arduino and ESP) or to the LongMessage buffer
* @param ForceRun Send a report instantly, even when regular reports are disabled
* @param ClearBuffer Flush the LongMessage buffer before starting to report
* @param KeepBuffer Stores the full JSON report in the LongMessage buffer - Only use this on the Mega2560 where LongMessage is large enough to store a complete report (Can be up to 1024kB)
* @param JSONToBufferOnly Do not print anything on the serial output, only fill the LongMessage buffer with the JSON report
*/
void Module::runReport(bool ForceRun, bool ClearBuffer, bool KeepBuffer, bool JSONToBufferOnly)
{
  if ((*SerialReportDate || ForceRun) && !JSONToBufferOnly)
  {
    getFormattedTime(true);
  }
  if ((*SerialReportMemory || ForceRun) && !JSONToBufferOnly)
  {
    getFreeMemory();
  }
  /*
  if ((*SerialReportJSONFriendly  || ForceRun) && !JSONToBufferOnly)
  {
    logToSerials(ReportQueueItemCount, false, 0); ///< Prints the number of items that will report
    logToSerials(F("reporting:"), true, 1);
    for (int i = 0; i < ReportQueueItemCount; i++)
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
    strcat_P(LongMessage, (PGM_P)F("{\"Log\":{")); ///< Adds two curly brackets that needs to be closed at the end
    if (!KeepBuffer)
    {
      logToSerials(&LongMessage, true, 0);
      memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    }
    for (int i = 0; i < ReportQueueItemCount;)
    {
      ReportQueue[i++]->report(JSONToBufferOnly || KeepBuffer ? false : *SerialReportJSONFriendly);
      if (i != ReportQueueItemCount)
        strcat_P(LongMessage, (PGM_P)F(",")); ///< < Unless it was the last element add a , separator
      if (!KeepBuffer)
      {
        logToSerials(&LongMessage, true, 0);
        memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
      }
    }
    strcat_P(LongMessage, (PGM_P)F("}}")); ///< closing both curly bracket
    if (!JSONToBufferOnly)
      logToSerials(&LongMessage, true, 0);
  }
}

///< Refresh queues: Refresh components inside the Module

void Module::run1sec()
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
      logToSerials(F("1sec"), true, 1);
    }
    for (int i = 0; i < RefreshQueueItemCount_Sec; i++)
    {
      RefreshQueue_Sec[i]->refresh_Sec();
    }
  }
}

void Module::run5sec()
{
  if (*Debug)
    logToSerials(F("5sec"), true, 1);
  for (int i = 0; i < RefreshQueueItemCount_FiveSec; i++)
  {
    RefreshQueue_FiveSec[i]->refresh_FiveSec();
  }
}

void Module::run1min()
{
  if (*Debug)
    logToSerials(F("1min"), true, 1);
  for (int i = 0; i < RefreshQueueItemCount_Minute; i++)
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
  if (QueueDepth > ReportQueueItemCount)
    ReportQueue[ReportQueueItemCount++] = Component;
  else
    logToSerials(F("Report OF"), true, 0); ///< Too many components are added to the queue, increase "QueueDepth" variable in Settings.h , or shift components to a different queue
}

void Module::addToRefreshQueue_Sec(Common *Component)
{
  if (QueueDepth > RefreshQueueItemCount_Sec)
    RefreshQueue_Sec[RefreshQueueItemCount_Sec++] = Component;
  else
    logToSerials(F("Refresh 1s OF"), true, 0);
}

void Module::addToRefreshQueue_FiveSec(Common *Component)
{
  if (QueueDepth > RefreshQueueItemCount_FiveSec)
    RefreshQueue_FiveSec[RefreshQueueItemCount_FiveSec++] = Component;
  else
    logToSerials(F("Refresh 5s OF"), true, 0);
}

void Module::addToRefreshQueue_Minute(Common *Component)
{
  if (QueueDepth > RefreshQueueItemCount_Minute)
    RefreshQueue_Minute[RefreshQueueItemCount_Minute++] = Component;
  else
    logToSerials(F("Refresh 1m OF"), true, 0);
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
  time_t Now = now();                                                                                                                          // Get the current time and date from the TimeLib library
  snprintf(CurrentTime, MaxWordLength, "%04d/%02d/%02d-%02d:%02d:%02d", year(Now), month(Now), day(Now), hour(Now), minute(Now), second(Now)); // YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
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