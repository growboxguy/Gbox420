#include "Reservoir.h"
#include "GrowBox.h"

//////////////////////////////////////////
//PHMeter functions

PHMeter::PHMeter(GrowBox * GBox, byte Pin, float &Intercept, float &Slope){
  this -> GBox = GBox;
  this -> Pin = Pin;
  this -> Intercept = Intercept;
  this -> Slope = Slope;
}

void PHMeter::getPH(bool ShowRaw){
  float  Reading=0;
  int PHRaw = analogRead(Pin);
  if(ShowRaw)
  {
  strncpy_P(Message,(PGM_P)F("PH analog read: "),MaxTextLength);
  strcat(Message,toText(PHRaw));
  GBox -> addToLog(Message);
  } 
  PH.updateAverage(Slope*PHRaw + Intercept); 
  //checkPHAlert();
}


void PHMeter::setSlope(float Value){
  Slope = Value;
  GBox -> addToLog(F("PH slope updated"));
}

void PHMeter::setIntercept(float Value){
  Intercept = Value;
  GBox -> addToLog(F("PH intercept updated"));
}
