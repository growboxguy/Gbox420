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

void PHSensor::getPH(bool ShowRaw){
  float  Reading=0;
  int PHRaw = analogRead(Pin);
  if(ShowRaw)
  {
    strncpy_P(Message,(PGM_P)F("PH analog read: "),MaxTextLength);
    strcat(Message,toText(PHRaw));
    GBox -> addToLog(Message);
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
