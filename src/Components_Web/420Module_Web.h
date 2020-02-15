#pragma once

#include "Arduino.h"     //every inheriting class have Arduino commands available
#include "ELClientRest.h" //ESP-link - REST API
#include "../../Settings.h"
#include "../Helpers.h"
#include "../Components/420Module.h"
#include "420Common_Web.h"

//This class represents a complete Module_Web with all of its components
//Responsible for setting up each module, updating their statuses and reporting it

extern ELClientRest PushingBoxRestAPI;
class Sound_Web;

class Module_Web : public Module
{
public:
  Module_Web(); //constructor
  void runReport();
  void runAll();
  void runSec();
  void runFiveSec();
  void runMinute();
  void runQuarterHour();
  time_t getNtpTime();
  void addToWebsiteQueue_Load(Common_Web *Component);
  void addToWebsiteQueue_Refresh(Common_Web *Component);
  void addToWebsiteQueue_Button(Common_Web *Component);
  void addToWebsiteQueue_Field(Common_Web *Component);
  void loadEvent(char *Url);
  void refreshEvent(char *Url);
  void buttonEvent(char *Button);
  void setFieldEvent(char *Field);
  void addToReportQueue(Common_Web *Component);
  void addToRefreshQueue_Sec(Common_Web *Component);
  void addToRefreshQueue_FiveSec(Common_Web *Component);
  void addToRefreshQueue_Minute(Common_Web *Component);
  void addToRefreshQueue_QuarterHour(Common_Web *Component);
  Sound_Web * getSoundObject();
  //void relayToGoogleSheets(__attribute__((unused)) const __FlashStringHelper *Title, __attribute__((unused)) char (*JSONData)[MaxLongTextLength]){};

private:

protected: 
  Common_Web *ReportQueue[QueueDepth] = {};  //aggregate initializer: Same as initializing to null pointers
  Common_Web *RefreshQueue_Sec[QueueDepth]= {};
  Common_Web *RefreshQueue_FiveSec[QueueDepth]= {};
  Common_Web *RefreshQueue_Minute[QueueDepth]= {};
  Common_Web *RefreshQueue_QuarterHour[QueueDepth]= {};
  Common_Web *WebsiteQueue_Load[QueueDepth] = {};
  Common_Web *WebsiteQueue_Refresh[QueueDepth]= {};
  Common_Web *WebsiteQueue_Button[QueueDepth]= {};
  Common_Web *WebsiteQueue_Field[QueueDepth]= {}; 
  byte reportQueueItemCount = 0; //Tracking queue item count
  byte refreshQueueItemCount_Sec = 0;
  byte refreshQueueItemCount_FiveSec = 0;
  byte refreshQueueItemCount_Minute = 0;
  byte refreshQueueItemCount_QuarterHour = 0; 
  byte WebsiteQueueItemCount_Load = 0;
  byte WebsiteQueueItemCount_Refresh = 0;
  byte WebsiteQueueItemCount_Button = 0;
  byte WebsiteQueueItemCount_Field = 0;
};