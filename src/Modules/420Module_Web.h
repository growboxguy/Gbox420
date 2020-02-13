#pragma once

#include "ELClientRest.h" //ESP-link - REST API
#include "../Components_Web/420Common_Web.h"
#include "420Module.h"

//This class represents a complete Module_Web with all of its components
//Responsible for setting up each module, updating their statuses and reporting it

extern ELClientRest PushingBoxRestAPI;

class Module_Web : virtual public Module
{
public:
  Module_Web(); //constructor
  void addToWebsiteQueue_Load(Common_Web *Component);
  void addToWebsiteQueue_Refresh(Common_Web *Component);
  void addToWebsiteQueue_Button(Common_Web *Component);
  void addToWebsiteQueue_Field(Common_Web *Component);
  void loadEvent(char *Url);
  void refreshEvent(char *Url);
  void buttonEvent(char *Button);
  void setFieldEvent(char *Field);

private:

protected: 
  Common_Web *WebsiteQueue_Load[QueueDepth] = {};
  Common_Web *WebsiteQueue_Refresh[QueueDepth]= {};
  Common_Web *WebsiteQueue_Button[QueueDepth]= {};
  Common_Web *WebsiteQueue_Field[QueueDepth]= {}; 
  byte WebsiteQueueItemCount_Load = 0;
  byte WebsiteQueueItemCount_Refresh = 0;
  byte WebsiteQueueItemCount_Button = 0;
  byte WebsiteQueueItemCount_Field = 0;
};