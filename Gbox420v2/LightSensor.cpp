#include "LightSensor.h"
#include "GrowBox.h"
#include "Lights.h"

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

int LightSensor::getReadingPercentage(){   
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
