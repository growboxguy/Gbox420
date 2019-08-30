#include "DHTSensor.h"
#include "GrowBox.h"

void DHTSensor::websiteRefreshEvent(){ //When the website is opened, load stuff once
  WebServer.setArgString(getWebsiteComponentName(F("Temp")), getTempText());
  WebServer.setArgString(getWebsiteComponentName(F("Humidity")), getHumidityText()); 
} 

DHTSensor::DHTSensor(const __FlashStringHelper * Name, GrowBox * GBox, uint8_t _pin, uint8_t _sensorType): Common(Name){
  this -> GBox = GBox;
  sensor = new DHT(_pin,_sensorType);
  sensor -> begin();  //dereference the pointer to the object and then call begin() on it. Same as (*sensor).begin();
  Temp = new RollingAverage();
  Humidity = new RollingAverage();
  logToSerials(F("DHT Sensor object created"),true);
}

void DHTSensor::refresh(){  //Called when component should refresh its state 
  Common::refresh();
  if(GBox -> BoxSettings -> MetricSystemEnabled){ Temp -> updateAverage(sensor -> readTemperature());}
  else {Temp -> updateAverage(sensor -> readTemperature() *1.8f + 32);}
  Humidity -> updateAverage(sensor -> readHumidity());  
  report();
}

void DHTSensor::report(){
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("Temp:")); strcat(LongMessage, getTempText());
  strcat_P(LongMessage,(PGM_P)F(" ; Humidity:")); strcat(LongMessage, getHumidityText());
  logToSerials(&LongMessage,true,4);
}

float DHTSensor::getTemp(){
  return Temp -> getAverageFloat();
}

float DHTSensor::getHumidity(){
  return Humidity -> getAverageFloat();
}

char* DHTSensor::getTempText(){
  return tempToText(Temp -> getAverageFloat());
}

char* DHTSensor::getHumidityText(){
  return percentageToText(Humidity -> getAverageFloat());
}
