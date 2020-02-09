#pragma once

#include "ELClientRest.h" //ESP-link - REST API
#include "420Common_Web.h"
#include "../Components/420Module.h"

//This class represents a complete Module_Web with all of its components
//Responsible for setting up each module, updating their statuses and reporting it

extern ELClientRest PushingBoxRestAPI;

class Module_Web : public Module
{
public:
  Module_Web(const __FlashStringHelper *Name); //constructor
  void AddToWebsiteQueue_Load(Common *Component);
  void AddToWebsiteQueue_Refresh(Common *Component);
  void AddToWebsiteQueue_Button(Common *Component);
  void AddToWebsiteQueue_Field(Common *Component);
  void loadEvent(char *Url);
  void refreshEvent(char *Url);
  void buttonEvent(char *Button);
  void setFieldEvent(char *Field);

private:
  Common *WebsiteQueue_Load[QueueDepth];
  Common *WebsiteQueue_Refresh[QueueDepth];
  Common *WebsiteQueue_Button[QueueDepth];
  Common *WebsiteQueue_Field[QueueDepth]; 
  byte WebsiteQueueItemCount_Load = 0;
  byte WebsiteQueueItemCount_Refresh = 0;
  byte WebsiteQueueItemCount_Button = 0;
  byte WebsiteQueueItemCount_Field = 0;

protected:  
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>Settings</h1>
  <button id="GBox1_RestoreDefaults" type="button" style="margin-top:0px" title="Restore settings to sketch defaults">Restore defaults</button>
  <form><label><input type="checkbox" name="GBox1_DebugEnabled" title="Debug entres on Serial log">Debug enabled </label><input type="submit" value="Set"></form>
  <form><label><input type="checkbox" name="GBox1_MetricSystemEnabled" title="Use imperial units(F,PSI) or metric (C,bar)">Metric units </label><input type="submit" value="Set"></form>				
</div>

<div class="card" style="width:90%">
  <h1>Reporting</h1>					
  <form><label><input type="checkbox" name="GBox1_SheetsEnabled">Google Sheets, every <input style="width: 55px;" min=15 max=1440 step=15 name="GBox1_SheetsFrequency" type="number"/> min </label><input type="submit" value="Set"></form>
  <form>RelayID:<input type="text" name="GBox1_PushingBoxLogRelayID"  style="width: 150px;" title="DeviceID of the PushingBox logging scenario"/><input type="submit" value="Set"></form>
</div>

<div class="pure-u-1 pure-u-md-1-2" style="max-width:370px">
  <div class="card" style="width:90%">	
    <h1><span id="Time"></span></h1>				
    <ul id="list_SerialLog"></ul>
    <button id="GBox1_Refresh" type="button" style="margin-top:0px" title="Updates the status of every module">Refresh</button>	
  </div>					
</div>
*/