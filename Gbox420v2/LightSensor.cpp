#include "LightSensor.h"

LightSensor::LightSensor(byte DigitalPin, byte AnalogPin){ //constructor
  this -> DigitalPin = DigitalPin;
  this -> AnalogPin = AnalogPin;
  pinMode(DigitalPin, INPUT);
  pinMode(AnalogPin, INPUT);
  LightReading = new RollingAverage();
  if(MySettings.DebugEnabled){logToSerials(F("LightSensor object created"),true);}
}

void LightSensor::refresh(){  //Called when component should refresh its state
  if(MySettings.DebugEnabled){logToSerials(F("LightSensor object refreshing"),true);}
  IsDark = digitalRead(DigitalPin); //digitalRead has negative logic: 0- light detected , 1 - no light detected. ! inverts this
  LightReading -> updateAverage((int)(1023 - analogRead(AnalogPin)));   
}

bool LightSensor::getIsDark(){ //Ligth sensor digital feedback: True(Dark) or False(Bright)  
  return IsDark;
}

int LightSensor::getReading(){ 
  return LightReading -> getAverageInt();
}

const __FlashStringHelper * LightSensor::getIsDarkText(){
  return yesNoToText(IsDark);
}

char* LightSensor::getReadingText(){
  return toText(LightReading -> getAverageInt());
}
