#include "ModuleSkeleton.h"
#include "GrowBox.h"

ModuleSkeleton::ModuleSkeleton (const __FlashStringHelper * Name, GrowBox * GBox,  Settings::ModuleSkeletonSettings * DefaultSettings): Common(Name){ //constructor
  this -> GBox = GBox;
  PersistentBool = &DefaultSettings -> PersistentBool;
  PersistentInt = &DefaultSettings -> PersistentInt;
  PersistentFloat = &DefaultSettings -> PersistentFloat;
  SmoothInt = new RollingAverage();
  SmoothFloat = new RollingAverage();

  GBox -> AddToReportQueue(this);  //Subscribing to the report queue: Calls the report() method
  GBox -> AddToRefreshQueue_Sec(this);  //Subscribing to refresh queue: Calls the refresh_Sec() method
  GBox -> AddToRefreshQueue_FiveSec(this);  //Subscribing to refresh queue: Calls the refresh_FiveSec() method
  GBox -> AddToRefreshQueue_Minute(this);  //Subscribing to refresh queue: Calls the refresh_Minute() method
  GBox -> AddToRefreshQueue_QuarterHour(this);  //Subscribing to refresh queue: Calls the refresh_QuarterHour() method

  GBox -> AddToWebsiteQueue_Load(this); //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  GBox -> AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  GBox -> AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  GBox -> AddToWebsiteQueue_Field(this); //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  logToSerials(F("ModuleSkeleton object created"),true,1); 
}

void ModuleSkeleton::report(){
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("PersistentBool:")); strcat(LongMessage, yesNoToText(*PersistentBool));
  strcat_P(LongMessage,(PGM_P)F(" ; PersistentInt:")); strcat(LongMessage, toText(*PersistentInt));
  strcat_P(LongMessage,(PGM_P)F(" ; PersistentFloat:")); strcat(LongMessage, toText(*PersistentFloat));  
  strcat_P(LongMessage,(PGM_P)F(" ; RuntimeBool:")); strcat(LongMessage, yesNoToText(RuntimeBool));
  strcat_P(LongMessage,(PGM_P)F(" ; RuntimeInt:")); strcat(LongMessage, toText(RuntimeInt));
  strcat_P(LongMessage,(PGM_P)F(" ; RuntimeFloat:")); strcat(LongMessage, toText(RuntimeFloat));
  strcat_P(LongMessage,(PGM_P)F(" ; RuntimeString:")); strcat(LongMessage, RuntimeString);
  strcat_P(LongMessage,(PGM_P)F(" ; SmoothInt:")); strcat(LongMessage, SmoothInt -> getAverageIntText());
  strcat_P(LongMessage,(PGM_P)F(" ; SmoothFloat:")); strcat(LongMessage, SmoothFloat -> getAverageFloatText());
  logToSerials(&LongMessage,true,1);
}  

void ModuleSkeleton::websiteEvent_Load(__attribute__((unused)) char * url){
  if (strcmp(url,"/Test.html.json")==0){
      WebServer.setArgBoolean(getWebsiteComponentName(F("PersistentBool")), *PersistentBool);
      WebServer.setArgInt(getWebsiteComponentName(F("PersistentInt")), *PersistentInt);
      WebServer.setArgFloat(getWebsiteComponentName(F("PersistentFloat")), *PersistentFloat);
      WebServer.setArgBoolean(getWebsiteComponentName(F("RuntimeBool")), RuntimeBool);
      WebServer.setArgInt(getWebsiteComponentName(F("RuntimeInt")), RuntimeInt);     
      WebServer.setArgFloat(getWebsiteComponentName(F("RuntimeFloat")), RuntimeFloat);
      WebServer.setArgString(getWebsiteComponentName(F("RuntimeString")), RuntimeString); 
      WebServer.setArgInt(getWebsiteComponentName(F("SmoothInt")), SmoothInt -> getAverageInt()); 
      WebServer.setArgFloat(getWebsiteComponentName(F("SmoothFloat")), SmoothFloat -> getAverageFloat()); 
  }
}

void ModuleSkeleton::websiteEvent_Refresh(__attribute__((unused)) char * url){
  if (strcmp(url,"/Test.html.json")==0){
      WebServer.setArgBoolean(getWebsiteComponentName(F("RuntimeBool")), RuntimeBool);
      WebServer.setArgInt(getWebsiteComponentName(F("RuntimeInt")), RuntimeInt);     
      WebServer.setArgFloat(getWebsiteComponentName(F("RuntimeFloat")), RuntimeFloat);
      WebServer.setArgString(getWebsiteComponentName(F("RuntimeString")), RuntimeString); 
      WebServer.setArgInt(getWebsiteComponentName(F("SmoothInt")), SmoothInt -> getAverageInt()); 
      WebServer.setArgFloat(getWebsiteComponentName(F("SmoothFloat")), SmoothFloat -> getAverageFloat());  
  }
}

void ModuleSkeleton::websiteEvent_Button(char * Button){ //When a button is pressed on the website
  if(!isThisMyComponent(Button)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name   
    if(strcmp_P(ShortMessage,(PGM_P)F("ResetFlash"))==0) {*PersistentBool = false; *PersistentInt = 420; *PersistentFloat = 4.2; }
    else if(strcmp_P(ShortMessage,(PGM_P)F("ResetRam"))==0) {RuntimeBool = false; RuntimeInt = 420; RuntimeFloat = 4.2; strncpy(RuntimeString, "HailMary",MaxTextLength); }  
  }  
} 

void ModuleSkeleton::websiteEvent_Field(char * Field){ //When the website is opened, load stuff once
  if(!isThisMyComponent(Field)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name     
    if(strcmp_P(ShortMessage,(PGM_P)F("SetBool"))==0) { RuntimeBool = WebServer.getArgBoolean(); } //Getting a bool
    else if(strcmp_P(ShortMessage,(PGM_P)F("SetString"))==0) { strncpy(RuntimeString, WebServer.getArgString(),MaxTextLength); } //Getting a string
    else if(strcmp_P(ShortMessage,(PGM_P)F("SetInt"))==0) { RuntimeInt = WebServer.getArgInt(); } //Getting an integer
    else if(strcmp_P(ShortMessage,(PGM_P)F("SetFloat"))==0) { RuntimeFloat = WebServer.getArgFloat(); }  //Getting a float
    else if(strcmp_P(ShortMessage,(PGM_P)F("PersistentInt"))==0) { *PersistentInt = WebServer.getArgInt(); } //Getting an integer
    else if(strcmp_P(ShortMessage,(PGM_P)F("PersistentFloat"))==0) { *PersistentFloat = WebServer.getArgFloat(); }  //Getting a float
    else if(strcmp_P(ShortMessage,(PGM_P)F("PersistentBool"))==0) { *PersistentBool = WebServer.getArgBoolean(); } //Getting a bool
      
  }  
} 


void ModuleSkeleton::refresh_Sec(){
  if(GBox -> BoxSettings -> DebugEnabled) Common::refresh_Sec();  
}

void ModuleSkeleton::refresh_FiveSec(){
  if(GBox -> BoxSettings -> DebugEnabled) Common::refresh_FiveSec();
  if(RuntimeBool) {
    RuntimeInt++;  
    RuntimeFloat += 0.1;
  }
}

void ModuleSkeleton::refresh_Minute(){
  if(GBox -> BoxSettings -> DebugEnabled) Common::refresh_Minute();  
}

void ModuleSkeleton::refresh_QuarterHour(){
  if(GBox -> BoxSettings -> DebugEnabled) Common::refresh_QuarterHour();  
}

