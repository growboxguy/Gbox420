#include "Lights.h"
#include "GrowBox.h"
#include "Sound.h"

Lights::Lights(const __FlashStringHelper * Name, GrowBox * GBox, Settings::LightsSettings * DefaultSettings) : Common(Name){
  this -> GBox = GBox;
  RelayPin = &DefaultSettings -> RelayPin;
  DimmingPin = &DefaultSettings -> DimmingPin;
  DimmingLimit = &DefaultSettings -> DimmingLimit; //Blocks dimming below a certain percentage (default 8%), Most LED drivers cannot fully dim, check the specification and adjust accordingly, only set 0 if it supports dimming fully! (Usually not the case..)
  Status = &DefaultSettings -> Status;
  Brightness = &DefaultSettings -> Brightness;
  TimerEnabled = &DefaultSettings -> TimerEnabled;
  OnHour = &DefaultSettings -> OnHour;
  OnMinute = &DefaultSettings -> OnMinute;
  OffHour = &DefaultSettings -> OffHour;
  OffMinute = &DefaultSettings -> OffMinute;
  pinMode(*RelayPin, OUTPUT);
  pinMode(*DimmingPin, OUTPUT);
  GBox -> AddToRefreshQueue_Minute(this);  //Subscribing to the Minute refresh queue: Calls the refresh() method  
  GBox -> AddToWebsiteQueue_Load(this);  //Subscribing to the Website load queue: Calls the websiteEvent_Load(__attribute__((unused)) char * url) method
  GBox -> AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event
  GBox -> AddToWebsiteQueue_Field(this); //Subscribing to the Website field submit event
  GBox -> AddToWebsiteQueue_Button(this); //Subscribing to the Website field submit event  
  logToSerials(F("Lights object created"),true);
}

void Lights::websiteEvent_Load(__attribute__((unused)) char * url){
  if (strcmp(url,"/GrowBox.html.json")==0){
    WebServer.setArgInt(getWebsiteComponentName(F("OnHour")), *OnHour); 
    WebServer.setArgInt(getWebsiteComponentName(F("OnMinute")),  *OnMinute); 
    WebServer.setArgInt(getWebsiteComponentName(F("OffHour")), *OffHour); 
    WebServer.setArgInt(getWebsiteComponentName(F("OffMinute")),*OffMinute);
    WebServer.setArgInt(getWebsiteComponentName(F("Brightness")), *Brightness);
    WebServer.setArgInt(getWebsiteComponentName(F("BrightnessSlider")), *Brightness);
  }
}

void Lights::websiteEvent_Refresh(__attribute__((unused)) char * url){
  if (strcmp(url,"/GrowBox.html.json")==0){
    WebServer.setArgString(getWebsiteComponentName(F("Status")),getStatusText());
    WebServer.setArgString(getWebsiteComponentName(F("TimerEnabled")), getTimerOnOffText());
    WebServer.setArgString(getWebsiteComponentName(F("OnTime")), getOnTimeText());
    WebServer.setArgString(getWebsiteComponentName(F("OffTime")), getOffTimeText());
  }
}

void Lights::websiteEvent_Field(char * Field){ //When the website is opened, load stuff once
  if(!isThisMyComponent(Field)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name     
    if(strcmp_P(ShortMessage,(PGM_P)F("Brightness"))==0) {setBrightness(WebServer.getArgInt(),true);}
    else if(strcmp_P(ShortMessage,(PGM_P)F("OnHour"))==0) {setOnHour(WebServer.getArgInt());}
    else if(strcmp_P(ShortMessage,(PGM_P)F("OnMinute"))==0) {setOnMinute(WebServer.getArgInt());}
    else if(strcmp_P(ShortMessage,(PGM_P)F("OffHour"))==0) {setOffHour(WebServer.getArgInt());}
    else if(strcmp_P(ShortMessage,(PGM_P)F("OffMinute"))==0) {setOffMinute(WebServer.getArgInt());} 
  }  
} 

void Lights::websiteEvent_Button(char * Button){ //When the website is opened, load stuff once
  if(!isThisMyComponent(Button)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name     
    if (strcmp_P(ShortMessage,(PGM_P)F("On"))==0) {setLightOnOff(true,true); }
      else if (strcmp_P(ShortMessage,(PGM_P)F("Off"))==0) {setLightOnOff(false,true); }
      else if (strcmp_P(ShortMessage,(PGM_P)F("TimerEnable"))==0) {setTimerOnOff(true);}
      else if (strcmp_P(ShortMessage,(PGM_P)F("TimerDisable"))==0) {setTimerOnOff(false);}
  }  
} 

void Lights::refresh(){  //makes the class non-virtual, by implementing the refresh function from Common (Else you get an error while trying to create a new Lights object: invalid new-expression of abstract class type 'Lights')
  Common::refresh();
  checkLightTimer(); 
  checkLightStatus();
  report();
}

void Lights::report(){
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("Status:")); strcat(LongMessage, getStatusText()); 
  strcat_P(LongMessage,(PGM_P)F(" ; Brightness:")); strcat(LongMessage,toText(*Brightness));
  strcat_P(LongMessage,(PGM_P)F(" ; LightON:")); strcat(LongMessage,getOnTimeText());
  strcat_P(LongMessage,(PGM_P)F(" ; LightOFF:")); strcat(LongMessage,getOffTimeText());
  logToSerials( &LongMessage, true,4);
}

void Lights::checkLightStatus(){
  if(*Status) digitalWrite(*RelayPin, LOW); //True turns relay ON (LOW signal activates Relay)
  else digitalWrite(*RelayPin, HIGH); //HIGH turns relay OFF 
}

void Lights::checkLightTimer() {
  if(*TimerEnabled){
    time_t Now = now();  // Get the current time from TimeLib
    int CombinedOnTime = *OnHour * 100 + *OnMinute; //convert time to number, Example: 8:10=810, 20:10=2010
    int CombinedOffTime = *OffHour * 100 + *OffMinute;
    int CombinedCurrentTime = hour(Now) * 100 + minute(Now);
    if(CombinedOnTime <= CombinedOffTime)  //no midnight turnover, Example: On 8:10, Off: 20:10
    {
      if(CombinedOnTime <= CombinedCurrentTime && CombinedCurrentTime < CombinedOffTime){  //True: Light should be on
        if(!*Status){   
          setLightOnOff(true,false); //If status is OFF: Turn ON the lights (First bool), and do not add it to the log (Second bool)
          if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Timer:Light ON"),true,4);
        } 
      }
      else  //False: Light should be off
        if(*Status) {  //If status is ON: Turn OFF the lights (First bool), and do not add it to the log (Second bool)
          setLightOnOff(false,false);    
          if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Timer:Light OFF"),true,4);
        }        
    }
    else   //midnight turnover, Example: On 21:20, Off: 9:20
    {
      if(CombinedOnTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedOffTime){
       if(!*Status) {
        setLightOnOff(true,false);
        if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Timer:Light ON"),true,4);
        }
      }
      else 
       if(*Status){
        setLightOnOff(false,false);
        if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Timer:Light OFF"),true,4);    
       }
    }
  }
}

void Lights::setBrightness(byte Brightness, bool LogThis){
  *(this -> Brightness) = Brightness;      
  analogWrite(*DimmingPin, map(Brightness,0,100,int(255*(100-*DimmingLimit)/100.0f),0)); //mapping brightness to duty cycle. Example 1: Mapping Brightness 100 -> PWM duty cycle will be 0% on Arduino side, 100% on LED driver side. Example2: Mapping Brightness 0 with Dimming limit 8% ->  int(255*((100-8)/100)) ~= 234 AnalogWrite (92% duty cycle on Arduino Side, 8% in Driver dimming side) https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
  if(LogThis){
    strncpy_P(LongMessage,(PGM_P)F("Brightness: "),MaxTextLength);  
    strcat(LongMessage,toText(Brightness));
    strcat_P(LongMessage,(PGM_P)F("%"));
    GBox -> addToLog(LongMessage);
  }
}

void Lights::setLightOnOff(bool Status, bool LogThis){
   *(this -> Status) = Status;
   if(LogThis){
      if(Status){
        GBox -> addToLog(F("Light ON"));
        GBox -> Sound1 -> playOnSound();
      }
      else{
        GBox -> addToLog(F("Light OFF")); 
        GBox -> Sound1 -> playOffSound();
      }
   }
   checkLightStatus();
}

void Lights::setTimerOnOff(bool TimerState){
  *(this -> TimerEnabled) = TimerState;
  if(*TimerEnabled){ 
    checkLightTimer();
    GBox -> addToLog(F("Timer enabled"));
    GBox -> Sound1 -> playOnSound();
    }
  else {
    GBox -> addToLog(F("Timer disabled"));
    GBox -> Sound1 -> playOffSound();
    }
}

void Lights::setOnHour(byte OnHour){
  *(this -> OnHour) = OnHour; 
}

void Lights::setOnMinute(byte OnMinute){
  *(this -> OnMinute) = OnMinute;
  GBox -> addToLog(F("Light ON time updated")); 
}

void Lights::setOffHour(byte OffHour){
  *(this -> OffHour) = OffHour;
}

void Lights::setOffMinute(byte OffMinute){
  *(this -> OffMinute) = OffMinute;
  GBox -> addToLog(F("Light OFF time updated"));
}

char *  Lights::getTimerOnOffText(){
  return enabledToText(*TimerEnabled);
}

bool Lights::getStatus(){
   return *Status;
}

char * Lights::getBrightness(){
    return toText(*Brightness);
}

char *  Lights::getStatusText(){
   return onOffToText(*Status);
}
    
char * Lights::getOnTimeText(){
   return timeToText(*OnHour,*OnMinute);
}
char * Lights::getOffTimeText(){
   return timeToText(*OffHour,*OffMinute);
}