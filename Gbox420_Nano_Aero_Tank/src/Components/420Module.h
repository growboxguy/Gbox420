#pragma once

#include "420Common.h"
#include "../../Settings.h"
#include "../Helpers.h"

//This class is the parent of all physical modules, like the Main / Hempy Bucket / Aeroponics / Environment module
//Responsible for creating and
//Adds 
class Sound;

class Module : virtual public Common
{
public:
  Module(); ///constructor
  
  //Module(const __FlashStringHelper *Name, Sound *SoundFeedback);
  void runReport(bool AddToLog = true);
  void runAll(bool AddToLog = true);
  void runSec(bool AddToLog = true);
  void runFiveSec(bool AddToLog = true);
  void runMinute(bool AddToLog = true);
  void runQuarterHour(bool AddToLog = true);
  virtual void addToLog(const __FlashStringHelper *Text, uint8_t indent = 3);
  virtual void addToLog(const char *Text, uint8_t indent = 3);  
  void addToReportQueue(Common *Component);   ///Subscribing to the report queue: Calls the report() method
  void addToRefreshQueue_Sec(Common *Component);  ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method
  void addToRefreshQueue_FiveSec(Common *Component);  ///Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  void addToRefreshQueue_Minute(Common *Component);   ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  void addToRefreshQueue_QuarterHour(Common *Component);
  char * getFormattedTime(bool PrintToSerials);
  Sound * getSoundObject();  
    
private:

protected:
  void setDebug(bool State);
  void setMetric(bool MetricEnabled);
  Sound* SoundFeedback = NULL;
  bool RunAllRequested = false;
  bool ConsoleReportRequested = false; 
  Common *ReportQueue[QueueDepth] = {};  ///aggregate initializer: Same as initializing to null pointers
  Common *RefreshQueue_Sec[QueueDepth]= {};
  Common *RefreshQueue_FiveSec[QueueDepth]= {};
  Common *RefreshQueue_Minute[QueueDepth]= {};
  Common *RefreshQueue_QuarterHour[QueueDepth]= {};
  uint8_t reportQueueItemCount = 0; ///Tracking queue item count
  uint8_t refreshQueueItemCount_Sec = 0;
  uint8_t refreshQueueItemCount_FiveSec = 0;
  uint8_t refreshQueueItemCount_Minute = 0;
  uint8_t refreshQueueItemCount_QuarterHour = 0; 
};