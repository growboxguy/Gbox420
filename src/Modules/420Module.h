#pragma once
#include "420Common.h"

//This class is the parent of all physical modules, like the Main / Hempy Bucket / Aeroponics / Environment module

class Module : public Common
{
public:
  Module(const __FlashStringHelper *Name); //constructor
  void runAll();
  void runSec();
  void runFiveSec();
  void runMinute();
  void runQuarterHour();
  void addToLog(const __FlashStringHelper *Text, byte indent = 3);
  void addToLog(const char *Text, byte indent = 3);  
  void AddToReportQueue(Common *Component);
  void AddToRefreshQueue_Sec(Common *Component);
  void AddToRefreshQueue_FiveSec(Common *Component);
  void AddToRefreshQueue_Minute(Common *Component);
  void AddToRefreshQueue_QuarterHour(Common *Component);
 
private:
  char *eventLogToJSON(bool Append = false); //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  void runReport();
  Common *ReportQueue[QueueDepth];
  Common *RefreshQueue_Sec[QueueDepth];
  Common *RefreshQueue_FiveSec[QueueDepth];
  Common *RefreshQueue_Minute[QueueDepth];
  Common *RefreshQueue_QuarterHour[QueueDepth];
  byte reportQueueItemCount = 0; //Tracking queue item count
  byte refreshQueueItemCount_Sec = 0;
  byte refreshQueueItemCount_FiveSec = 0;
  byte refreshQueueItemCount_Minute = 0;
  byte refreshQueueItemCount_QuarterHour = 0;

protected:
  bool RunAllRequested = false;
  bool ConsoleReportRequested = false; 
};