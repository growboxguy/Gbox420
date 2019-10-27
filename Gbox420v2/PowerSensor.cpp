#include "PowerSensor.h"
#include "GrowBox.h"

PowerSensor::PowerSensor(const __FlashStringHelper * Name, GrowBox * GBox,HardwareSerial * SerialPort):Common(Name){
  this -> GBox = GBox;
  Sensor = new PZEM004Tv30(SerialPort);
  GBox -> AddToRefreshQueue_Minute(this);  //Subscribing to the Minute refresh queue: Calls the refresh() method
  GBox -> AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event
  logToSerials(F("PowerSensor object created"),true);
}

void PowerSensor::websiteEvent_Refresh(char * url){ //When the website is opened, load stuff once
  if (strcmp(url,"/GrowBox.html.json")==0){
    WebServer.setArgString(getWebsiteComponentName(F("Power")), getPowerText());  
    WebServer.setArgString(getWebsiteComponentName(F("Energy")), getEnergyText());  
    WebServer.setArgString(getWebsiteComponentName(F("Voltage")), getVoltageText());  
    WebServer.setArgString(getWebsiteComponentName(F("Current")), getCurrentText());  
    WebServer.setArgString(getWebsiteComponentName(F("Frequency")), getFrequencyText());  
    WebServer.setArgString(getWebsiteComponentName(F("PowerFactor")), getPowerFactorText());
  }
} 

void PowerSensor::refresh(){
  Common::refresh();
  Voltage = Sensor -> voltage();  //AC Voltage (V)
  Current = Sensor -> current(); //Current (A)
  Power = Sensor -> power(); //Actual power usage (W)
  Energy = Sensor -> energy() / 1000;  //total power consumption (kWh)
  Frequency = Sensor -> frequency();  //total power consumption (kWh)
  PowerFactor = Sensor -> pf();  //total power consumption (kWh)
  report();
}

void PowerSensor::report(){
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("Power:")); strcat(LongMessage,toText(Power)); strcat_P(LongMessage,(PGM_P)F("W")); 
  strcat_P(LongMessage,(PGM_P)F(" ; Total:")); strcat(LongMessage,toText(Energy)); strcat_P(LongMessage,(PGM_P)F("Wh"));   
  strcat_P(LongMessage,(PGM_P)F(" ; Voltage:")); strcat(LongMessage,toText(Voltage)); strcat_P(LongMessage,(PGM_P)F("V"));
  strcat_P(LongMessage,(PGM_P)F(" ; Current:")); strcat(LongMessage,toText(Current)); strcat_P(LongMessage,(PGM_P)F("A"));
  strcat_P(LongMessage,(PGM_P)F(" ; Frequency:")); strcat(LongMessage,toText(Frequency)); strcat_P(LongMessage,(PGM_P)F("Hz"));
  strcat_P(LongMessage,(PGM_P)F(" ; PowerFactor:")); strcat(LongMessage,toText(PowerFactor));
  logToSerials( &LongMessage, true,4);
}
 
char * PowerSensor::getPowerText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(Power));   
  strcat_P(ReturnChar,(PGM_P)F("W"));  
  return ReturnChar;    
}

char * PowerSensor::getEnergyText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(Energy));   
  strcat_P(ReturnChar,(PGM_P)F("kWh"));  
  return ReturnChar;    
}
    
char * PowerSensor::getVoltageText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(Voltage));   
  strcat_P(ReturnChar,(PGM_P)F("V"));  
  return ReturnChar;    
}

char * PowerSensor::getCurrentText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(Current));   
  strcat_P(ReturnChar,(PGM_P)F("A"));  
  return ReturnChar;      
}
    
char * PowerSensor::getFrequencyText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(Frequency));   
  strcat_P(ReturnChar,(PGM_P)F("Hz"));  
  return ReturnChar;      
}

char * PowerSensor::getPowerFactorText(){
  return toText(PowerFactor);      
}