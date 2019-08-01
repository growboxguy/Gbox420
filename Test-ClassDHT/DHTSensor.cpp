#include "DHTSensor.h"



 void DHTSensor::refresh(){  //Called when component should refresh its state 
    if(MySettings.MetricSystemEnabled){ Temp -> updateAverage((float)(sensor -> readTemperature()));}
    else {Temp -> updateAverage( (float)(sensor -> readTemperature() *1.8 + 32));}
    Humidity -> updateAverage((float)(sensor -> readHumidity()));
  }

  DHTSensor::DHTSensor(byte _pin, uint8_t _sensorType){
    sensor = new DHT(_pin,_sensorType);
    sensor -> begin();  //dereference the pointer to the object and then call begin() on it. Same as (*sensor).begin();
    Temp = new RollingAverage();
    Humidity = new RollingAverage();
  }

  float DHTSensor::getTemp(){
    return Temp -> getAverageFloat();
  }

  float DHTSensor::getHumidity(){
    return Humidity -> getAverageFloat();
  }

  char* DHTSensor::getTempText(){
    return  tempToText(Temp -> getAverageFloat());
  }
  
  char* DHTSensor::getHumidityText(){
    return  percentageToText(Humidity -> getAverageFloat());
  }
