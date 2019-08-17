#include "Lights.h"
#include "Buzzer.h"

Lights::Lights(GrowBox * GBox,byte RelayPin, byte DimmingPin, byte* DimmingLimit, bool *Status, byte *Brightness, bool *TimerEnabled, byte *OnHour, byte *OnMinute, byte *OffHour, byte *OffMinute){  //constructor
  this -> GBox = GBox;
  this -> RelayPin = RelayPin;
  this -> DimmingPin = DimmingPin;
  this -> DimmingLimit = DimmingLimit;
  this -> Status = Status;
  this -> Brightness = Brightness;
  this -> TimerEnabled = TimerEnabled;
  this -> OnHour = OnHour;
  this -> OnMinute = OnMinute;
  this -> OffHour = OffHour;
  this -> OffMinute = OffMinute;
  pinMode(RelayPin, OUTPUT);
  pinMode(DimmingPin, OUTPUT);
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("Lights object created"),true);}
}

void Lights::refresh(){  //makes the class non-virtual, by implementing the refresh function from Common (Else you get an error while trying to create a new Lights object: invalid new-expression of abstract class type 'Lights')
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("Lights refreshing"),true);}
  checkLightTimer(); 
  checkLightStatus(); 
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
          if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Timer:Light ON"),true);
        } 
      }
      else  //False: Light should be off
        if(*Status) {  //If status is ON: Turn OFF the lights (First bool), and do not add it to the log (Second bool)
          setLightOnOff(false,false);    
          if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Timer:Light OFF"),true);
        }        
    }
    else   //midnight turnover, Example: On 21:20, Off: 9:20
    {
      if(CombinedOnTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedOffTime){
       if(!*Status) {
        setLightOnOff(true,false);
        if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Timer:Light ON"),true);
        }
      }
      else 
       if(*Status){
        setLightOnOff(false,false);
        if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Timer:Light OFF"),true);    
       }
    }
  }
}

void Lights::setBrightness(byte Brightness, bool LogThis){
  *(this -> Brightness) = Brightness;      
  analogWrite(*DimmingPin, map(Brightness,0,100,int(255*(100-*DimmingLimit)/100.0f),0)); //mapping brightness to duty cycle. Example 1: Mapping Brightness 100 -> PWM duty cycle will be 0% on Arduino side, 100% on LED driver side. Example2: Mapping Brigthness 0 with Dimming limit 8% ->  int(255*((100-8)/100)) ~= 234 AnalogWrite (92% duty cycle on Arduino Side, 8% in Driver dimming side) https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
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
        GBox -> Buzzer1 -> playOnSound();
      }
      else{
        GBox -> addToLog(F("Light OFF")); 
        GBox -> Buzzer1 -> playOffSound();
      }
   }
   checkLightStatus();
}

void Lights::setTimerOnOff(bool TimerState){
  *(this -> TimerEnabled) = TimerState;
  if(*TimerEnabled){ 
    checkLightTimer();
    GBox -> addToLog(F("Timer enabled"));
    GBox -> Buzzer1 -> playOnSound();
    }
  else {
    GBox -> addToLog(F("Timer disabled"));
    GBox -> Buzzer1 -> playOffSound();
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

__FlashStringHelper* Lights::getTimerOnOffText(){
  return enabledToText(*TimerEnabled);
}

__FlashStringHelper* Lights::getStatusText(){
   return onOffToText(*Status);
}
    
char * Lights::getOnTimeText(){
   return timeToText(*OnHour,*OnMinute);
}
char * Lights::getOffTimeText(){
   return timeToText(*OffHour,*OffMinute);
}
