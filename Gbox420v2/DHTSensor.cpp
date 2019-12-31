#include "DHTSensor.h"
#include "GrowBox.h"

DHTSensor::DHTSensor(const __FlashStringHelper * Name, GrowBox * GBox, Settings::DHTSensorSettings * DefaultSettings): Common(Name){
  this -> GBox = GBox;
  Sensor = new DHT(*(&DefaultSettings -> Pin),*(&DefaultSettings -> Type));
  Sensor -> begin();  //dereference the pointer to the object and then call begin() on it. Same as (*Sensor).begin();
  Temp = new RollingAverage();
  Humidity = new RollingAverage();
  GBox -> AddToRefreshQueue_Minute(this);  //Subscribing to the Minute refresh queue: Calls the refresh() method
  GBox -> AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event
  logToSerials(F("DHT Sensor object created"),true,1);
}

void DHTSensor::websiteEvent_Refresh(__attribute__((unused)) char * url){ //When the website is opened, load stuff once
  if (strcmp(url,"/GrowBox.html.json")==0){
    WebServer.setArgString(getWebsiteComponentName(F("Temp")), getTempText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Humidity")), getHumidityText(true));
  } 
} 

void DHTSensor::refresh_Minute(){  //Called when component should refresh its state 
  Common::refresh_Minute();
  if(GBox -> BoxSettings -> MetricSystemEnabled){ Temp -> updateAverage(Sensor -> readTemperature());}
  else {Temp -> updateAverage(Sensor -> readTemperature() *1.8f + 32);}
  Humidity -> updateAverage(Sensor -> readHumidity());  
  report();
}

void DHTSensor::report(){
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("Temp:")); strcat(LongMessage, getTempText(true));
  strcat_P(LongMessage,(PGM_P)F(" ; Humidity:")); strcat(LongMessage, getHumidityText(true));
  logToSerials(&LongMessage,true,4);
}

float DHTSensor::getTemp(){
  return Temp -> getAverageFloat();
}

float DHTSensor::getHumidity(){
  return Humidity -> getAverageFloat();
}

char * DHTSensor::getTempText(bool IncludeUnits){
  if(IncludeUnits) return tempToText(Temp -> getAverageFloat());
  else return Temp -> getAverageFloatText();
}

char * DHTSensor::getHumidityText(bool IncludeUnits){  
  if(IncludeUnits) return percentageToText(Humidity -> getAverageFloat());
  else return Humidity -> getAverageFloatText();
}