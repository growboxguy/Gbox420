#include "Fan.h"
#include "../../GrowBox.h"
#include "Sound.h"

Fan::Fan(const __FlashStringHelper * Name, GrowBox * GBox, Settings::FanSettings * DefaultSettings): Common(Name){
  this -> GBox = GBox;
  OnOffPin = &DefaultSettings -> OnOffPin;
  SpeedPin = &DefaultSettings -> SpeedPin;
  State = &DefaultSettings -> State;
  HighSpeed = &DefaultSettings -> HighSpeed;
  pinMode(*OnOffPin, OUTPUT);
  pinMode(*SpeedPin, OUTPUT);
  GBox -> AddToReportQueue(this);  //Subscribing to the report queue: Calls the report() method
  GBox -> AddToRefreshQueue_Minute(this);  //Subscribing to the Minute refresh queue: Calls the refresh_Minute() method
  GBox -> AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event
  GBox -> AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event
  logToSerials(F("Fan object created"),true,1);
}

void Fan::websiteEvent_Refresh(__attribute__((unused)) char * url){ //When the website is opened, load stuff once
  if (strcmp(url,"/GrowBox.html.json")==0){
    WebServer.setArgString(getWebsiteComponentName(F("Status")), fanSpeedToText());
  } 
} 

void Fan::websiteEvent_Button(char * Button){ //When the website is opened, load stuff once
  if(!isThisMyComponent(Button)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name     
    if (strcmp_P(ShortMessage,(PGM_P)F("Off"))==0) {TurnOff();}
    else if (strcmp_P(ShortMessage,(PGM_P)F("Low"))==0) {SetLowSpeed();}
    else if (strcmp_P(ShortMessage,(PGM_P)F("High"))==0) {SetHighSpeed();}
  }  
} 

void Fan::refresh_Minute(){
  if(GBox -> BoxSettings -> DebugEnabled) Common::refresh_Minute();
  checkFanStatus();
}

void Fan::report(){
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("Status:")); strcat(LongMessage,toText(*State));
  strcat_P(LongMessage,(PGM_P)F(" ; High:")); strcat(LongMessage,toText(*HighSpeed));
  strcat_P(LongMessage,(PGM_P)F(" ; Text:")); strcat(LongMessage,fanSpeedToText());
  strcat_P(LongMessage,(PGM_P)F(" ; Number:")); strcat(LongMessage,fanSpeedToNumber());
  logToSerials( &LongMessage, true,1);
}

void Fan::checkFanStatus(){
 if(*State) digitalWrite(*OnOffPin, LOW); else digitalWrite(*OnOffPin, HIGH); //True turns relay ON (LOW signal activates Relay)
 if(*HighSpeed) digitalWrite(*SpeedPin, LOW); else digitalWrite(*SpeedPin, HIGH);
}

void Fan::TurnOff(){
  *State = false;
  *HighSpeed = false;
  checkFanStatus();
  WebServer.setArgString(getWebsiteComponentName(F("Status")), fanSpeedToText());
  GBox -> addToLog(F("Fan OFF"));
  GBox -> Sound1 -> playOffSound();
}

void Fan::SetLowSpeed(){
  *State = true;
  *HighSpeed = false;
  checkFanStatus();
  WebServer.setArgString(getWebsiteComponentName(F("Status")), fanSpeedToText());
  GBox -> addToLog(F("Fan speed LOW"));
  GBox -> Sound1 -> playOnSound();
}

void Fan::SetHighSpeed(){
  *State = true;
  *HighSpeed = true;
  checkFanStatus();
  WebServer.setArgString(getWebsiteComponentName(F("Status")), fanSpeedToText());
  GBox -> addToLog(F("Fan speed HIGH"));
  GBox -> Sound1 -> playOnSound();
}

char * Fan::fanSpeedToText(){
   if(!*State) return (char *)"OFF";
   else if (*HighSpeed) return (char *)"HIGH";
   else return (char *)"LOW";
}

char * Fan::fanSpeedToNumber(){
   if(!*State) return (char *)"0";
   else if (*HighSpeed) return (char *)"2";
   else return (char *)"1";
}

// WebServer.setArgBoolean(F("AutoInFan"),GBox -> BoxSettings -> AutomaticInFan);
// WebServer.setArgString(F("InFanSwitchTemp"), toText(GBox -> BoxSettings -> InFanSwitchTemp));
// WebServer.setArgBoolean(F("AutoExFan"), GBox -> BoxSettings -> AutomaticExFan);
// WebServer.setArgString(F("ExFanHighHumid"), toText(GBox -> BoxSettings -> ExFanHighHumid));
// WebServer.setArgString(F("ExFanLowHumid"), toText(GBox -> BoxSettings -> ExFanLowHumid));
// WebServer.setArgString(F("ExFanOffHumid"), toText(GBox -> BoxSettings -> ExFanOffHumid));