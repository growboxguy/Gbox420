#include "DHTSensor.h"
#include "GrowBox.h"

void DHTSensor::refresh(){  //Called when component should refresh its state 
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("DHTSensor refreshing"),true);}    
  if(GBox -> BoxSettings -> MetricSystemEnabled){ Temp -> updateAverage(sensor -> readTemperature());}
  else {Temp -> updateAverage(sensor -> readTemperature() *1.8f + 32);}
  Humidity -> updateAverage(sensor -> readHumidity());  
  report();
}

void DHTSensor::report(){
  memset(&Message[0], 0, sizeof(Message));  //clear variable
  strcat_P(Message,(PGM_P)F("Temp:")); strcat(Message, getTempText());
  strcat_P(Message,(PGM_P)F(" ; Humidity:")); strcat(Message, getHumidityText());
  logToSerials(&Message,true,4);
}

DHTSensor::DHTSensor(GrowBox * GBox, uint8_t _pin, uint8_t _sensorType){
  this -> GBox = GBox;
  sensor = new DHT(_pin,_sensorType);
  sensor -> begin();  //dereference the pointer to the object and then call begin() on it. Same as (*sensor).begin();
  Temp = new RollingAverage();
  Humidity = new RollingAverage();
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("DHT Sensor object created"),true);}
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
