#include "Lights.h"
#include "GrowBox.h"
#include "Sound.h"

//////////////////////////////////////////////////////////////////
//Lights functions

Lights::Lights(const __FlashStringHelper * Name, GrowBox * GBox, byte RelayPin, byte DimmingPin, Settings::LightsSettings * DefaultSettings, byte DimmingLimit) : Common(Name){
  this -> GBox = GBox;
  this -> RelayPin = RelayPin;
  this -> DimmingPin = DimmingPin;
  this -> DimmingLimit = DimmingLimit; //Blocks dimming below a certain percentage (default 8%), Most LED drivers cannot fully dim, check the specification and adjust accordingly, only set 0 if it supports dimming fully! (Usually not the case..)
  Status = &DefaultSettings -> Status;
  Brightness = &DefaultSettings -> Brightness;
  TimerEnabled = &DefaultSettings -> TimerEnabled;
  OnHour = &DefaultSettings -> OnHour;
  OnMinute = &DefaultSettings -> OnMinute;
  OffHour = &DefaultSettings -> OffHour;
  OffMinute = &DefaultSettings -> OffMinute;
  pinMode(RelayPin, OUTPUT);
  pinMode(DimmingPin, OUTPUT);
  logToSerials(F("Lights object created"),true);
}

void Lights::refresh(){  //makes the class non-virtual, by implementing the refresh function from Common (Else you get an error while trying to create a new Lights object: invalid new-expression of abstract class type 'Lights')
  Common::refresh();
  checkLightTimer(); 
  checkLightStatus();
  report();
}

void Lights::report(){
  memset(&Message[0], 0, sizeof(Message));  //clear variable
  strcat_P(Message,(PGM_P)F("Status:")); strcat(Message, getStatusText()); 
  strcat_P(Message,(PGM_P)F(" ; Brightness:")); strcat(Message,toText(*Brightness));
  strcat_P(Message,(PGM_P)F(" ; LightON:")); strcat(Message,getOnTimeText());
  strcat_P(Message,(PGM_P)F(" ; LightOFF:")); strcat(Message,getOffTimeText());
  logToSerials( &Message, true,4);
}

void Lights::websiteLoadEvent(){
  //Light1 load
  WebServer.setArgInt(getWebsiteComponentName(F("OnHour")), *OnHour); 
  WebServer.setArgInt(getWebsiteComponentName(F("OnMinute")),  *OnMinute); 
  WebServer.setArgInt(getWebsiteComponentName(F("OffHour")), *OffHour); 
  WebServer.setArgInt(getWebsiteComponentName(F("OffMinute")),*OffMinute);
  WebServer.setArgInt(getWebsiteComponentName(F("Brightness")), *Brightness);
  WebServer.setArgInt(getWebsiteComponentName(F("BrightnessSlider")), *Brightness);
}

void Lights::websiteRefreshEvent(){
//Light1
  WebServer.setArgString(getWebsiteComponentName(F("Status")),getStatusText());
  WebServer.setArgString(getWebsiteComponentName(F("TimerEnabled")), getTimerOnOffText());
  WebServer.setArgString(getWebsiteComponentName(F("OnTime")), getOnTimeText());
  WebServer.setArgString(getWebsiteComponentName(F("OffTime")), getOffTimeText()); 
}


void Lights::checkLightStatus(){
  if(*Status) digitalWrite(RelayPin, LOW); //True turns relay ON (LOW signal activates Relay)
  else digitalWrite(RelayPin, HIGH); //HIGH turns relay OFF 
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
  analogWrite(DimmingPin, map(Brightness,0,100,int(255*(100-DimmingLimit)/100.0f),0)); //mapping brightness to duty cycle. Example 1: Mapping Brightness 100 -> PWM duty cycle will be 0% on Arduino side, 100% on LED driver side. Example2: Mapping Brightness 0 with Dimming limit 8% ->  int(255*((100-8)/100)) ~= 234 AnalogWrite (92% duty cycle on Arduino Side, 8% in Driver dimming side) https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
  if(LogThis){
    strncpy_P(Message,(PGM_P)F("Brightness: "),MaxTextLength);  
    strcat(Message,toText(Brightness));
    strcat_P(Message,(PGM_P)F("%"));
    GBox -> addToLog(Message);
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

char *  Lights::getStatusText(){
   return onOffToText(*Status);
}
    
char * Lights::getOnTimeText(){
   return timeToText(*OnHour,*OnMinute);
}
char * Lights::getOffTimeText(){
   return timeToText(*OffHour,*OffMinute);
}
