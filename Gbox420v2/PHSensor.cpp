#include "PHSensor.h"
#include "GrowBox.h"

PHSensor::PHSensor(GrowBox * GBox, byte Pin, float * Intercept, float * Slope){
  this -> GBox = GBox;
  this -> Pin = Pin;
  this -> Intercept = Intercept;
  this -> Slope = Slope;
  pinMode(Pin, INPUT);
  PH = new RollingAverage();
  GBox -> AddToRefreshQueue_FiveSec(this);  //Subscribing to the FiveSec refresh queue: Calls the refresh() method
}

void PHSensor::websiteEvent_Load(__attribute__((unused)) char * url){ //When the website is opened, load stuff once
  // WebServer.setArgInt(getWebsiteComponentName(F("PumpTimeout")), *SprayEnabled);
  // WebServer.setArgInt(getWebsiteComponentName(F("PrimingTime")), *Interval);
  // WebServer.setArgInt(getWebsiteComponentName(F("Interval")), *Interval);
  // WebServer.setArgInt(getWebsiteComponentName(F("Duration")), *Duration); 
} 

void PHSensor::refresh(){
  Common::refresh();
  if(GBox -> BoxSettings -> DebugEnabled){ getPH(true); }
  else { getPH(false); }
}

void PHSensor::getPH(bool ShowRaw){
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