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
  void runReport();
  void runAll();
  void runSec();
  void runFiveSec();
  void runMinute();
  void runQuarterHour();
  void addToLog(const __FlashStringHelper *Text, uint8_t indent = 3);
  void addToLog(const char *Text, uint8_t indent = 3);  
  void addToReportQueue(Common *Component);
  void addToRefreshQueue_Sec(Common *Component);
  void addToRefreshQueue_FiveSec(Common *Component);
  void addToRefreshQueue_Minute(Common *Component);
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