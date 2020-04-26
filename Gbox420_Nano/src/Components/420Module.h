#pragma once

#include "420Common.h"
#include "../../Settings.h"
#include "../../Wireless.h"
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
  void addToLog(const __FlashStringHelper *Text, byte indent = 3);
  void addToLog(const char *Text, byte indent = 3);  
  void addToReportQueue(Common *Component);
  void addToRefreshQueue_Sec(Common *Component);
  void addToRefreshQueue_FiveSec(Common *Component);
  void addToRefreshQueue_Minute(Common *Component);
  void addToRefreshQueue_QuarterHour(Common *Component);
  char * getFormattedTime(bool PrintToSerials);
  Sound * getSoundObject();  
    
private:

protected:
  Sound* SoundFeedback = NULL;
  bool RunAllRequested = false;
  bool ConsoleReportRequested = false; 
  Common *ReportQueue[QueueDepth] = {};  ///aggregate initializer: Same as initializing to null pointers
  Common *RefreshQueue_Sec[QueueDepth]= {};
  Common *RefreshQueue_FiveSec[QueueDepth]= {};
  Common *RefreshQueue_Minute[QueueDepth]= {};
  Common *RefreshQueue_QuarterHour[QueueDepth]= {};
  byte reportQueueItemCount = 0; ///Tracking queue item count
  byte refreshQueueItemCount_Sec = 0;
  byte refreshQueueItemCount_FiveSec = 0;
  byte refreshQueueItemCount_Minute = 0;
  byte refreshQueueItemCount_QuarterHour = 0; 
};