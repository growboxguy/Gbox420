#include "PowerSensor.h"
#include "GrowBox.h"

PowerSensor::PowerSensor(const __FlashStringHelper * Name, GrowBox * GBox,HardwareSerial * SerialPort):Common(Name){
  this -> GBox = GBox;
  Sensor = new PZEM004Tv30(SerialPort);
  GBox -> AddToReportQueue(this);  //Subscribing to the report queue: Calls the report() method
  GBox -> AddToRefreshQueue_Minute(this);  //Subscribing to the Minute refresh queue: Calls the refresh_Minute() method
  GBox -> AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event
  logToSerials(F("PowerSensor object created"),true,1);
}

void PowerSensor::websiteEvent_Refresh(__attribute__((unused)) char * url){ //When the website is opened, load stuff once
  if (strcmp(url,"/GrowBox.html.json")==0){
    WebServer.setArgString(getWebsiteComponentName(F("Power")), getPowerText(true));  
    WebServer.setArgString(getWebsiteComponentName(F("Energy")), getEnergyText(true));  
    WebServer.setArgString(getWebsiteComponentName(F("Voltage")), getVoltageText(true));  
    WebServer.setArgString(getWebsiteComponentName(F("Current")), getCurrentText(true));  
    WebServer.setArgString(getWebsiteComponentName(F("Frequency")), getFrequencyText(true));  
    WebServer.setArgString(getWebsiteComponentName(F("PowerFactor")), getPowerFactorText());
  }
} 

void PowerSensor::refresh_Minute(){
  if(GBox -> BoxSettings -> DebugEnabled) Common::refresh_Minute();
  Voltage = Sensor -> voltage();  //AC Voltage (V)
  Current = Sensor -> current(); //Current (A)
  Power = Sensor -> power(); //Actual power usage (W)
  Energy = Sensor -> energy() / 1000;  //total power consumption (kWh)
  Frequency = Sensor -> frequency();  //total power consumption (kWh)
  PowerFactor = Sensor -> pf();  //total power consumption (kWh)
}

void PowerSensor::report(){
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("Power:")); strcat(LongMessage,getPowerText(true)); 
  strcat_P(LongMessage,(PGM_P)F(" ; Total:")); strcat(LongMessage,getEnergyText(true));   
  strcat_P(LongMessage,(PGM_P)F(" ; Voltage:")); strcat(LongMessage,getVoltageText(true));
  strcat_P(LongMessage,(PGM_P)F(" ; Current:")); strcat(LongMessage,getCurrentText(true));
  strcat_P(LongMessage,(PGM_P)F(" ; Frequency:")); strcat(LongMessage,getFrequencyText(true));
  strcat_P(LongMessage,(PGM_P)F(" ; PowerFactor:")); strcat(LongMessage,getPowerFactorText());
  logToSerials( &LongMessage, true,1);
}
 
char * PowerSensor::getPowerText(bool IncludeUnits){
  if(IncludeUnits){
    static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
    memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
    strcat(ReturnChar,toText(Power));   
    if(IncludeUnits) strcat_P(ReturnChar,(PGM_P)F("W"));  
    return ReturnChar;
  }
  else return toText(Power);  
}

char * PowerSensor::getEnergyText(bool IncludeUnits){
  if(IncludeUnits){
    static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
    memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
    strcat(ReturnChar,toText(Energy));   
    if(IncludeUnits) strcat_P(ReturnChar,(PGM_P)F("kWh"));  
    return ReturnChar;
  }
  else return toText(Energy);  
}
    
char * PowerSensor::getVoltageText(bool IncludeUnits){
  if(IncludeUnits){
    static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
    memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
    strcat(ReturnChar,toText(Voltage));   
    if(IncludeUnits) strcat_P(ReturnChar,(PGM_P)F("V"));  
    return ReturnChar;
  }
  else return toText(Voltage);   
}

char * PowerSensor::getCurrentText(bool IncludeUnits){
  if(IncludeUnits){
    static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
    memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
    strcat(ReturnChar,toText(Current));   
    if(IncludeUnits) strcat_P(ReturnChar,(PGM_P)F("A")); 
    return ReturnChar;
  }
  else return toText(Current);
}
    
char * PowerSensor::getFrequencyText(bool IncludeUnits){
  if(IncludeUnits){
    static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
    memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
    strcat(ReturnChar,toText(Frequency));   
    strcat_P(ReturnChar,(PGM_P)F("Hz"));  
    return ReturnChar;
  }
  else return toText(Frequency);
}

char * PowerSensor::getPowerFactorText(){
  return toText(PowerFactor);      
}