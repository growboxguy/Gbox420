#pragma once

#include "420Common.h"
#include "../../Settings.h"
#include "../Helpers.h"

///< Common parent class of all modules

class Sound;

class Module : virtual public Common
{
public:
  Module(const __FlashStringHelper *Name); ///< constructor
  void reportToSerialTrigger(bool ForceRun = false, bool ClearBuffer = true, bool KeepBuffer = false, bool JSONToBufferOnly = false);
  void runReport(bool ForceRun = false, bool ClearBuffer = true, bool KeepBuffer = false, bool JSONOnly = false); ///< Generate a text log of all sensor readings to the Serial output and/or to the LongMessage buffer.
  void runAll();
  void run1sec();
  void run5sec();
  void run1min();
  virtual void addToLog(const __FlashStringHelper *Text, uint8_t indent = 3);
  virtual void addToLog(const char *Text, uint8_t indent = 3);
  void addToReportQueue(Common *Component);          ///< Subscribing to the report queue: Calls the report() method
  void addToRefreshQueue_Sec(Common *Component);     ///< Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method
  void addToRefreshQueue_FiveSec(Common *Component); ///< Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  void addToRefreshQueue_Minute(Common *Component);  ///< Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  char *getFormattedTime(bool PrintToSerials);
  Sound *SoundFeedback = NULL;
  Sound *getSoundObject();
  uint16_t &SerialReportFrequency;   ///< Frequency of Serial reports in seconds
  uint16_t SerialTriggerCounter = 0; ///< Helps with timing when to send the Serial report out
  bool &SerialReportDate;            ///< Enable/disable reporting the current time to the Serial output
  bool &SerialReportMemory;          ///< Enable/disable reporting the remaining free memory to the Serial output
  bool &SerialReportJSONFriendly;    ///< Enable/disable sending Text formatted reports to the Serial output
  bool &SerialReportJSON;            ///< Enable/disable sending JSON formatted reports to the Serial output
  bool &SerialReportWireless;        ///< Enable/disable sending wireless package exchange reports to the Serial output

private:
protected:
  void setDebug(bool State);
  void setMetric(bool MetricEnabled);
  void setSerialReportingFrequency(uint16_t Frequency); ///< Set the of Frequency of Serial reports in seconds (5 sec increments, 5 sec minimum)
  void setSerialReportDate(bool State);                 ///< Enable/disable reporting the current time to the Serial output
  void setSerialReportMemory(bool State);               ///< Enable/disable reporting the remaining free memory to the Serial output
  void setSerialReportJSON(bool State);                 ///< Enable/disable sending JSON formatted reports to the Serial output
  void setSerialReportJSONFriendly(bool State);         ///< Enable/disable sending JSON report with friendly values (Sec,%,Min,kg/lbs..etc appended) to Serial
  void setSerialReportWireless(bool State);             ///< Enable/disable sending wireless package exchange reports to the Serial output

  bool RunAllRequested = false;
  bool ConsoleReportRequested = false;
  Common *ReportQueue[QueueDepth] = {}; ///< aggregate initializer: Same as initializing to null pointers
  Common *RefreshQueue_Sec[QueueDepth] = {};
  Common *RefreshQueue_FiveSec[QueueDepth] = {};
  Common *RefreshQueue_Minute[QueueDepth] = {};
  uint8_t ReportQueueItemCount = 0; ///< Tracking queue item count
  uint8_t RefreshQueueItemCount_Sec = 0;
  uint8_t RefreshQueueItemCount_FiveSec = 0;
  uint8_t RefreshQueueItemCount_Minute = 0;
};