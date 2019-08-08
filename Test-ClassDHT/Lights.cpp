#include "Lights.h"

Lights::Lights(byte RelayPin, byte DimmingPin, byte* DimmingLimit, bool *Status, byte *Brightness, bool *TimerEnabled, byte *OnHour, byte *OnMinute, byte *OffHour, byte *OffMinute){  //constructor
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
  if(MySettings.DebugEnabled){logToSerials(F("Lights object created"),true);}
}

void Lights::refresh(){  //makes the class non-virtual, by implementing the refresh function from Common (Else you get an error while trying to create a new Lights object: invalid new-expression of abstract class type 'Lights')
 if(MySettings.DebugEnabled){logToSerials(F("Lights refreshing"),true);}

    if(CalibrateLights){ calibrateLights(); } //If calibration was requested
    void checkLightTimer(); 
    void checkLightStatus(); 
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
        if(!*Status) setLightOnOff(true,false);} //If status is OFF: Turn ON the lights (First bool), and do not add it to the log (Second bool)
      else  //False: Light should be off
        if(*Status) setLightOnOff(false,false);  //If status is ON: Turn OFF the lights (First bool), and do not add it to the log (Second bool)  
    }
    else   //midnight turnover, Example: On 21:20, Off: 9:20
    {
      if(CombinedOnTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedOffTime){
       if(!*Status) setLightOnOff(true,false);}
      else 
        if(*Status) setLightOnOff(false,false);    
    }
  }
}

void Lights::setBrightness(byte Brightness, bool LogThis){
  *(this -> Brightness) = Brightness;      
  analogWrite(*DimmingPin, map(Brightness,0,100,int(255.0*(100.0-(float)*DimmingLimit)/100.0),0)); //mapping brightness to duty cycle. Example 1: Mapping Brightness 100 -> PWM duty cycle will be 0% on Arduino side, 100% on LED driver side. Example2: Mapping Brigthness 0 with Dimming limit 8% ->  int(255*((100-8)/100)) ~= 234 AnalogWrite (92% duty cycle on Arduino Side, 8% in Driver dimming side) https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
  if(LogThis){
    strncpy_P(Message,(PGM_P)F("Brightness: "),MaxTextLength);  
    strcat(Message,toText(Brightness));
    strcat_P(Message,(PGM_P)F("%"));
    //addToLog(Message);
  }
}

void Lights::triggerCalibrateLights(){ //website signals to calibrate lights the next time "checkLightStatus" function runs
  CalibrateLights = true; 
}

void Lights::calibrateLights(){
  CalibrateLights=false;  
  bool* LastStatus = Status;
  byte* LastBrightness = Brightness;
  setLightOnOff(true,false);
  checkLightStatus();  //apply turning the lights on
  setBrightness(0,false);
  delay(500); //wait for light output change
  MinReading = 1023 - analogRead(LightSensorAnalogInPin);
  setBrightness(100,false);
  delay(500); //wait for light output change
  MaxReading = 1023 - analogRead(LightSensorAnalogInPin);
  
  if(MySettings.DebugEnabled){
         //logToSerials(F("0% - "),false); logToSerials(*MinReading,true);
         //logToSerials(F(", 100% - "),false); logToSerials(*MaxReading,true);
  }
  setBrightness(LastBrightness,false); //restore original brightness
  Status=LastStatus; //restore original state
  checkLightStatus();
  //addToLog(F("Lights calibrated"));
}

void Lights::setLightOnOff(bool Status, bool LogThis){
   *(this -> Status) = Status;
   if(LogThis){
      if(Status){
        //addToLog(F("Light ON"));
        //PlayOnSound=true;
      }
      else{
        //addToLog(F("Light OFF")); 
        //PlayOffSound=true;
      }
   }
}

void Lights::setTimerOnOff(bool TimerState){
  *(this -> TimerEnabled) = TimerState;
  if(TimerEnabled){ 
    checkLightTimer();
    //addToLog(F("Timer enabled"));
   // PlayOnSound=true;
    }
  else {
    //addToLog(F("Timer disabled"));
   // PlayOffSound=true;
    }
}

void Lights::setOnHour(byte OnHour){
  *(this -> OnHour) = OnHour; 
}

void Lights::setOnMinute(byte OnMinute){
  *(this -> OnMinute) = OnMinute;
  //addToLog(F("Light ON time updated")); 
  logToSerials(F("Light ON time updated"),false);
  //logToSerials(timeToText(*OnHour,*OnMinute),true);
}

void Lights::setOffHour(byte OffHour){
  *(this -> OffHour) = OffHour;
}

void Lights::setOffMinute(byte OffMinute){
  *(this -> OffMinute) = OffMinute;
  //addToLog(F("Light OFF time updated"));
  logToSerials(F("Light OFF time updated"),false);
//  logToSerials(timeToText(*OffHour,*OffMinute),true);
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
