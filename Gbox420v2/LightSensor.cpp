#include "LightSensor.h"

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
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("LightSensor object refreshing"),true);}
  if(CalibrateRequested){ calibrate(); } //If calibration was requested
  IsDark = digitalRead(DigitalPin); //digitalRead has negative logic: 0- light detected , 1 - no light detected. ! inverts this
  LightReading -> updateAverage(1023 - analogRead(AnalogPin));   
}

bool LightSensor::getIsDark(){ //Ligth sensor digital feedback: True(Dark) or False(Bright)  
  return IsDark;
}

int LightSensor::getReading(){ 
  return LightReading -> getAverageInt();
}

int LightSensor::getReadingPercentage(){ 
  return percentageToText(LightReading -> getAverageInt()); //CALIBRATION NOT IMPLEMENTED 
}

const __FlashStringHelper * LightSensor::getIsDarkText(){
  return yesNoToText(IsDark);
}

char* LightSensor::getReadingText(){
  return toText(LightReading -> getAverageInt());
}

void LightSensor::triggerCalibration(){ //website signals to calibrate light sensor MAX and MIN readings the next time a refresh runs
  CalibrateRequested = true; 
}

void LightSensor::calibrate(){
  CalibrateRequested=false;  
  bool * LastStatus = Status;
  byte * LastBrightness = Brightness;
  setLightOnOff(true,false);  //turn on light, without adding a log entry
  checkLightStatus();  //apply turning the lights on
  setBrightness(0,false);
  delay(250); //wait for light output change
  MinReading = 1023 - analogRead(GBox -> BoxSettings -> LightSensor1AnalogPin);
  setBrightness(100,false);
  delay(250); //wait for light output change
  MaxReading = 1023 - analogRead(GBox -> BoxSettings -> LightSensor1AnalogPin);
  
  if(GBox -> BoxSettings -> DebugEnabled){
         logToSerials(F("0% - "),false); logToSerials(&MinReading,true);
         logToSerials(F(", 100% - "),false); logToSerials(&MaxReading,true);
  }
  setBrightness(LastBrightness,false); //restore original brightness
  Status=LastStatus; //restore original state
  checkLightStatus();
  GBox -> addToLog(F("Lights calibrated"));
}
