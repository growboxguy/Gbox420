#include "420Helpers.h"
#include "GrowBox.h"

void resetWebServer(void) {    // Callback made from esp-link to notify that it has just come out of a reset
  logToSerials(F("(re)Connecting ESP-link.."),true,0);
  while(!ESPLink.Sync())  {
    logToSerials(F("."),false,0);
    delay(500); 
    };
  if(PushingBoxRestAPI.begin("api.pushingbox.com") == 0){logToSerials(F("PushingBox RestAPI ready"),true,2);} //Pre-setup relay to Google Sheets 
  else logToSerials(F("PushingBox RestAPI failed"),true,2); //If begin returns a negative number the initialization failed
  WebServer.setup();
  URLHandler *GrowBoxHandler = WebServer.createURLHandler("/GrowBox.html.json"); //setup handling request from GrowBox.html
  URLHandler *SettingsHandler = WebServer.createURLHandler("/Settings.html.json"); //setup handling request from Settings.html
  URLHandler *TestHandler = WebServer.createURLHandler("/Test.html.json"); //setup handling request from Test.html
  GrowBoxHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  GrowBoxHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
  SettingsHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  SettingsHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  SettingsHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  SettingsHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
  TestHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  TestHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  TestHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  TestHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
  logToSerials(F("ESP-link connected"),true,0);
}

void loadCallback(__attribute__((unused)) char * url){ //called when website is loaded.
  for(int i=0;i<GBox -> WebsiteQueueItemCount_Load;i++){
    GBox -> WebsiteQueue_Load[i] -> websiteEvent_Load(url);
  } 
}

void refreshCallback(__attribute__((unused)) char * url){ //called when website is refreshed.
  for(int i=0;i<GBox -> WebsiteQueueItemCount_Refresh;i++){
    GBox -> WebsiteQueue_Refresh[i] -> websiteEvent_Refresh(url);
  }
}

void buttonPressCallback(char *button){  //Called when any button on the website is pressed.
  if(GBox -> BoxSettings -> DebugEnabled)logToSerials(&button,true,0);
  for(int i=0;i<GBox -> WebsiteQueueItemCount_Button;i++){
    GBox -> WebsiteQueue_Button[i] -> websiteEvent_Button(button);
  }
  if (strcmp_P(ShortMessage,(PGM_P)F("RestoreDefaults"))==0) { restoreDefaults(GBox -> BoxSettings); }  
  saveSettings(false,GBox -> BoxSettings); 
}

void setFieldCallback(char * field){  //Called when any field on the website is updated.
  if(GBox -> BoxSettings -> DebugEnabled)logToSerials(&field,true,0);   
  for(int i=0;i<GBox -> WebsiteQueueItemCount_Field;i++){
    GBox -> WebsiteQueue_Field[i] -> websiteEvent_Field(field);
  }     
  saveSettings(false,GBox -> BoxSettings);
} 