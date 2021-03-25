#pragma once

#include "420Common.h"
#include "../../Settings.h"
#include "../Helpers.h"

//This class is the parent of all physical modules, like the Main / Hempy Bucket / Aeroponics / Environment module

class Sound;

class Module : virtual public Common
{
public:
  Module(); ///< constructor
  //Module(const __FlashStringHelper *Name, Sound *SoundFeedback);
  void runReport(bool JSONFormat = false, bool ClearBuffer = true, bool KeepBuffer = false); ///< Generate a text log of all sensor readings to the Serial output. If JSONFormat is enabled the report is printed as a JSON format.
  void runAll();
  void runSec();
  void runFiveSec();
  void runMinute();
  virtual void addToLog(const __FlashStringHelper *Text, uint8_t indent = 3);
  virtual void addToLog(const char *Text, uint8_t indent = 3);
  void addToReportQueue(Common *Component);          ///< Subscribing to the report queue: Calls the report() method
  void addToRefreshQueue_Sec(Common *Component);     ///< Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method
  void addToRefreshQueue_FiveSec(Common *Component); ///< Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  void addToRefreshQueue_Minute(Common *Component);  ///< Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  char *getFormattedTime(bool PrintToSerials);
  Sound *getSoundObject();
  bool *JSONToSerialMode; ///< Enable/disable sending JSON formatted reports to the Serial output

private:
protected:
  void setDebug(bool State);
  void setMetric(bool MetricEnabled);
  void setJSONToSerial(bool JSONEnabled);  
  Sound *SoundFeedback = NULL;
  bool RunAllRequested = false;
  bool ConsoleReportRequested = false;
  Common *ReportQueue[QueueDepth] = {}; ///< aggregate initializer: Same as initializing to null pointers
  Common *RefreshQueue_Sec[QueueDepth] = {};
  Common *RefreshQueue_FiveSec[QueueDepth] = {};
  Common *RefreshQueue_Minute[QueueDepth] = {};
  uint8_t reportQueueItemCount = 0; ///< Tracking queue item count
  uint8_t refreshQueueItemCount_Sec = 0;
  uint8_t refreshQueueItemCount_FiveSec = 0;
  uint8_t refreshQueueItemCount_Minute = 0;
};