#include "PHSensor.h"
#include "GrowBox.h"

//////////////////////////////////////////
//PHSensor functions

PHSensor::PHSensor(GrowBox * GBox, byte Pin, float * Intercept, float * Slope){
  this -> GBox = GBox;
  this -> Pin = Pin;
  this -> Intercept = Intercept;
  this -> Slope = Slope;
  pinMode(Pin, INPUT);
  PH = new RollingAverage();
}

void PHSensor::refresh(){
  Common::refresh();
  if(GBox -> BoxSettings -> DebugEnabled){ getPH(true); }
  else { getPH(false); }
}

void PHSensor::websiteLoadEvent(){ //When the website is opened, load stuff once
  // WebServer.setArgInt(getWebsiteComponentName(F("PumpTimeout")), *SprayEnabled);
  // WebServer.setArgInt(getWebsiteComponentName(F("PrimingTime")), *Interval);
  // WebServer.setArgInt(getWebsiteComponentName(F("Interval")), *Interval);
  // WebServer.setArgInt(getWebsiteComponentName(F("Duration")), *Duration); 
} 

void PHSensor::getPH(bool ShowRaw){
  float  Reading=0;
  int PHRaw = analogRead(Pin);
  if(ShowRaw)
  {
    strncpy_P(LongMessage,(PGM_P)F("PH analog read: "),MaxTextLength);
    strcat(LongMessage,toText(PHRaw));
    GBox -> addToLog(LongMessage);
  } 
  PH -> updateAverage((*Slope)*PHRaw + (*Intercept)); 
  //checkPHAlert();
}


void PHSensor::setSlope(float Value){
  *Slope = Value;
  GBox -> addToLog(F("PH slope updated"));
}

void PHSensor::setIntercept(float Value){
  *Intercept = Value;
  GBox -> addToLog(F("PH intercept updated"));
}
