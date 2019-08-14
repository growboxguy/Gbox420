#include "GrowBox.h"

GrowBox::GrowBox(Settings *BoxSettings){ //Constructor
  this -> BoxSettings = BoxSettings;
  InternalDHTSensor = new DHTSensor(BoxSettings -> InternalDHTSensorPin,DHT22);  //passing: Pin and Sensor type: DHT22 or DHT11)
  ExternalDHTSensor = new DHTSensor(BoxSettings -> ExternalDHTSensorPin,DHT22);  //passing: Pin and Sensor type: DHT22 or DHT11)
  LightSensor1 = new LightSensor(BoxSettings ->  LightSensor1DigitalPin, BoxSettings ->  LightSensor1AnalogPin);
  Light1 = new Lights(BoxSettings -> Light1RelayPin,BoxSettings -> Light1DimmingPin,&BoxSettings -> Light1DimmingLimit,&BoxSettings -> Light1Status,&BoxSettings -> Light1Brightness,&BoxSettings -> Light1TimerEnabled,&BoxSettings -> Light1OnHour,&BoxSettings -> Light1OnMinute,&BoxSettings -> Light1OffHour,&BoxSettings -> Light1OffMinute);   //Passing BoxSettings members as references: Changes get written back to BoxSettings and saved to EEPROM. (byte *)(((byte *)&BoxSettings) + offsetof(Settings, LightOnHour))
  addToLog(F("Grow Box initialized"));
  if(MySettings.DebugEnabled){logToSerials(F("GrowBox object created"),true);}
}

void GrowBox::refresh(){  //implementing the virtual refresh function from Common
 logToSerials(F("GrowBox object refreshing"),true);
    //triger all threads at startup
  runFiveSec(); //needs to run first to get sensor readings
  runSec();  
  runMinute();
  runHalfHour();
}

void GrowBox::runSec(){ 
  
}

void GrowBox::runFiveSec(){
  InternalDHTSensor -> refresh();
  ExternalDHTSensor -> refresh();
  LightSensor1 -> refresh();
}

void GrowBox::runMinute(){
  Light1 -> refresh();
}

void GrowBox::runHalfHour(){
    
}
