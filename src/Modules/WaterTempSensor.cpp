#include "WaterTempSensor.h"
#include "../../GrowBox.h"

WaterTempSensor::WaterTempSensor (const __FlashStringHelper * Name, GrowBox * GBox,  Settings::WaterTempSensorSettings * DefaultSettings): Common(Name){ //constructor
  this -> GBox = GBox;
  //pinMode(*Pin, INPUT);
  Temp = new RollingAverage();
  TempSensorWire = new OneWire(*(&DefaultSettings -> Pin)); //Reservoir waterproof temperature sensor (DS18B20)
  TempSensor = new DallasTemperature(TempSensorWire); //Reservoir waterproof temperature sensor (DS18B20)
  TempSensor -> begin();
  GBox -> AddToReportQueue(this);  //Subscribing to the report queue: Calls the report() method
  GBox -> AddToRefreshQueue_Minute(this);  //Subscribing to the FiveSec refresh queue: Calls the refresh() method
  GBox -> AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event 
  logToSerials(F("WaterTempSensor object created"),true,1); 
}

void WaterTempSensor::websiteEvent_Refresh(__attribute__((unused)) char * url){
  if (strcmp(url,"/GrowBox.html.json")==0){
      WebServer.setArgString(getWebsiteComponentName(F("Temp")), getTempText(true,false));  
  }
}

void WaterTempSensor::refresh_Minute(){
  if(GBox -> BoxSettings -> DebugEnabled) Common::refresh_Minute();
  TempSensor -> requestTemperatures(); 
  if(GBox -> BoxSettings -> MetricSystemEnabled){ Temp -> updateAverage(TempSensor -> getTempCByIndex(0)); }
  else {Temp -> updateAverage(TempSensor -> getTempFByIndex(0));}  
}

void WaterTempSensor::report(){
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("Temp:")); strcat(LongMessage, getTempText(true,true));
  logToSerials(&LongMessage,true,1);
}  

float WaterTempSensor::getTemp(bool ReturnAverage = true){
  return Temp -> getFloat(ReturnAverage);
}

char * WaterTempSensor::getTempText(bool IncludeUnits = true, bool ReturnAverage = true){
  if(IncludeUnits) return tempToText(Temp -> getFloat(ReturnAverage));
  else return Temp -> getFloatText(ReturnAverage);
}