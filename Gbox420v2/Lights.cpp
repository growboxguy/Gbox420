#include "Lights.h"
#include "GrowBox.h"
#include "Sound.h"

//////////////////////////////////////////////////////////////////
//Lights functions

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
  report();
}

void Lights::report(){
  memset(&Message[0], 0, sizeof(Message));  //clear variable
  strcat_P(Message,(PGM_P)F("Status:")); strcat_P(Message,(PGM_P) getStatusText()); 
  strcat_P(Message,(PGM_P)F(" ; Brightness:")); strcat(Message,toText(*Brightness));
  strcat_P(Message,(PGM_P)F(" ; LightON:")); strcat(Message,getOnTimeText());
  strcat_P(Message,(PGM_P)F(" ; LightOFF:")); strcat(Message,getOffTimeText());
  logToSerials( &Message, true,4);
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
  analogWrite(DimmingPin, map(Brightness,0,100,int(255*(100-*DimmingLimit)/100.0f),0)); //mapping brightness to duty cycle. Example 1: Mapping Brightness 100 -> PWM duty cycle will be 0% on Arduino side, 100% on LED driver side. Example2: Mapping Brigthness 0 with Dimming limit 8% ->  int(255*((100-8)/100)) ~= 234 AnalogWrite (92% duty cycle on Arduino Side, 8% in Driver dimming side) https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
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

const __FlashStringHelper* Lights::getTimerOnOffText(){
  return enabledToText(*TimerEnabled);
}

byte Lights::getBrightness(){
   return *Brightness;
}

bool Lights::getStatus(){
   return *Status;
}

const __FlashStringHelper* Lights::getStatusText(){
   return onOffToText(*Status);
}
    
char * Lights::getOnTimeText(){
   return timeToText(*OnHour,*OnMinute);
}
char * Lights::getOffTimeText(){
   return timeToText(*OffHour,*OffMinute);
}

//////////////////////////////////////////////////////////////////
//LightSensor functions


LightSensor::LightSensor(GrowBox * GBox, byte DigitalPin, byte AnalogPin){ //constructor
  this -> GBox = GBox;
  this -> DigitalPin = DigitalPin;
  this -> AnalogPin = AnalogPin;
  pinMode(DigitalPin, INPUT);
  pinMode(AnalogPin, INPUT);
  LightReading = new RollingAverage();
  triggerCalibration();
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("LightSensor object created"),true);}
}

void LightSensor::refresh(){  //Called when component should refresh its state
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("LightSensor refreshing"),true);}
  if(CalibrateRequested){ calibrate(); } //If calibration was requested
  IsDark = digitalRead(DigitalPin); //digitalRead has negative logic: 0- light detected , 1 - no light detected. ! inverts this
  LightReading -> updateAverage(1023 - analogRead(AnalogPin)); 
  report();
}

void LightSensor::report(){
  memset(&Message[0], 0, sizeof(Message));  //clear variable
  strcat_P(Message,(PGM_P)F("Light detected:")); strcat_P(Message,(PGM_P) getIsDarkText());
  strcat_P(Message,(PGM_P)F(" ; LightReading:")); strcat(Message, getReadingText());
  strcat_P(Message,(PGM_P)F(" (")); strcat(Message, getReadingPercentage());strcat_P(Message,(PGM_P)F(")"));
  logToSerials( &Message, true,4);
}

bool LightSensor::getIsDark(){ //Ligth sensor digital feedback: True(Dark) or False(Bright)  
  return IsDark;
}

int LightSensor::getReading(){ 
  return LightReading -> getAverageInt();
}

char * LightSensor::getReadingPercentage(){   
  return percentageToText(map(LightReading -> getAverageInt(),MinReading,MaxReading,0,100)); //https://www.arduino.cc/reference/en/language/functions/math/map/ 
}

const __FlashStringHelper * LightSensor::getIsDarkText(){
  return yesNoToText(IsDark);
}

char* LightSensor::getReadingText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,LightReading -> getAverageIntText());   
  strcat_P(ReturnChar,(PGM_P)F(" [")); 
  strcat(ReturnChar,toText(MinReading));
  strcat_P(ReturnChar,(PGM_P)F("/"));
  strcat(ReturnChar,toText(MaxReading));
  strcat_P(ReturnChar,(PGM_P)F("]"));   
  return ReturnChar;
}

void LightSensor::triggerCalibration(){ //website signals to calibrate light sensor MAX and MIN readings the next time a refresh runs
  CalibrateRequested = true; 
}

void LightSensor::calibrate(){
  CalibrateRequested=false;  
  bool LastStatus = GBox -> Light1 -> getStatus();  //TODO: This should be more generic and support different Lights objects passed as a parameter
  byte LastBrightness = GBox -> Light1 -> getBrightness();
  GBox -> Light1 -> setLightOnOff(true,false);  //turn on light, without adding a log entry
  GBox -> Light1 -> checkLightStatus();  //apply turning the lights on
  GBox -> Light1 -> setBrightness(0,false);
  delay(500); //wait for light output change
  MinReading = 1023 - analogRead(AnalogPin);
  GBox -> Light1 -> setBrightness(100,false);
  delay(500); //wait for light output change
  MaxReading = 1023 - analogRead(AnalogPin);
  GBox -> Light1 -> setBrightness(LastBrightness,false); //restore original brightness, without adding a log entry
  GBox -> Light1 -> setLightOnOff(LastStatus,false); //restore original state, without adding a log entry
  GBox -> Light1 -> checkLightStatus();
  GBox -> addToLog(F("Lights calibrated"),4);
  if(GBox -> BoxSettings -> DebugEnabled){
         logToSerials(F("0% - "),false,4); logToSerials(&MinReading,false,0);
         logToSerials(F(", 100% - "),false,0); logToSerials(&MaxReading,true,0);
  }
}
