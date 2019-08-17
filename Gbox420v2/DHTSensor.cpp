#include "DHTSensor.h"

 void DHTSensor::refresh(){  //Called when component should refresh its state 
    if(MySettings.DebugEnabled){logToSerials(F("DHTSensor refreshing"),true);}    
    if(MySettings.MetricSystemEnabled){ Temp -> updateAverage(sensor -> readTemperature());}
    else {Temp -> updateAverage(sensor -> readTemperature() *1.8f + 32);}
    Humidity -> updateAverage(sensor -> readHumidity());  
  }

  DHTSensor::DHTSensor(uint8_t _pin, uint8_t _sensorType){
    sensor = new DHT(_pin,_sensorType);
    sensor -> begin();  //dereference the pointer to the object and then call begin() on it. Same as (*sensor).begin();
    Temp = new RollingAverage();
    Humidity = new RollingAverage();
    if(MySettings.DebugEnabled){logToSerials(F("DHT Sensor object created"),true);}
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
