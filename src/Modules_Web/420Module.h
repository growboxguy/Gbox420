#pragma once
#include "420Common_Web.h"

//This class is the parent of all physical modules, like the Main / Hempy Bucket / Aeroponics / Environment module

class Module : public Common_Web
{
public:
  Module(const __FlashStringHelper *Name); //constructor
  void runReport();
  void runAll();
  void runSec();
  void runFiveSec();
  void runMinute();
  void runQuarterHour();
  void addToLog(const __FlashStringHelper *Text, byte indent = 3);
  void addToLog(const char *Text, byte indent = 3);  
  void AddToReportQueue(Common_Web *Component);
  void AddToRefreshQueue_Sec(Common_Web *Component);
  void AddToRefreshQueue_FiveSec(Common_Web *Component);
  void AddToRefreshQueue_Minute(Common_Web *Component);
  void AddToRefreshQueue_QuarterHour(Common_Web *Component);
 
private:
  char *eventLogToJSON(bool Append = false); //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  Common_Web *ReportQueue[QueueDepth];
  Common_Web *RefreshQueue_Sec[QueueDepth];
  Common_Web *RefreshQueue_FiveSec[QueueDepth];
  Common_Web *RefreshQueue_Minute[QueueDepth];
  Common_Web *RefreshQueue_QuarterHour[QueueDepth];
  byte reportQueueItemCount = 0; //Tracking queue item count
  byte refreshQueueItemCount_Sec = 0;
  byte refreshQueueItemCount_FiveSec = 0;
  byte refreshQueueItemCount_Minute = 0;
  byte refreshQueueItemCount_QuarterHour = 0;

protected:
  bool RunAllRequested = false;
  bool ConsoleReportRequested = false; 
};