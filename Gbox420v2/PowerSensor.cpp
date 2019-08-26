#include "PowerSensor.h"
#include "GrowBox.h"

PowerSensor::PowerSensor(const __FlashStringHelper * Name, GrowBox * GBox,HardwareSerial * SerialPort):Common(Name){
  this -> GBox = GBox;
  Sensor = new PZEM004Tv30(SerialPort);
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("PowerSensor object created"),true);} 
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
  memset(&Message[0], 0, sizeof(Message));  //clear variable
  strcat_P(Message,(PGM_P)F("Power:")); strcat(Message,toText(Power)); strcat_P(Message,(PGM_P)F("W")); 
  strcat_P(Message,(PGM_P)F(" ; Total:")); strcat(Message,toText(Energy)); strcat_P(Message,(PGM_P)F("Wh"));   
  strcat_P(Message,(PGM_P)F(" ; Voltage:")); strcat(Message,toText(Voltage)); strcat_P(Message,(PGM_P)F("V"));
  strcat_P(Message,(PGM_P)F(" ; Current:")); strcat(Message,toText(Current)); strcat_P(Message,(PGM_P)F("A"));
  strcat_P(Message,(PGM_P)F(" ; Frequency:")); strcat(Message,toText(Frequency)); strcat_P(Message,(PGM_P)F("Hz"));
  strcat_P(Message,(PGM_P)F(" ; PowerFactor:")); strcat(Message,toText(PowerFactor));
  logToSerials( &Message, true,4);
}

float PowerSensor::getPower(){
  return Power;    
}

float PowerSensor::getEnergy(){
  return Energy;    
}

float PowerSensor::getVoltage(){
  return Voltage;    
}

float PowerSensor::getCurrent(){
  return Current;    
}

float PowerSensor::getFrequency(){
  return Frequency;    
}

float PowerSensor::getPowerFactor(){
  return PowerFactor;    
}

char * PowerSensor::getPowerText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(getPower()));   
  strcat_P(ReturnChar,(PGM_P)F("W"));  
  return ReturnChar;    
}

char * PowerSensor::getEnergyText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(getEnergy()));   
  strcat_P(ReturnChar,(PGM_P)F("kWh"));  
  return ReturnChar;    
}
    
char * PowerSensor::getVoltageText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(getVoltage()));   
  strcat_P(ReturnChar,(PGM_P)F("V"));  
  return ReturnChar;    
}

char * PowerSensor::getCurrentText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(getCurrent()));   
  strcat_P(ReturnChar,(PGM_P)F("A"));  
  return ReturnChar;      
}
    
char * PowerSensor::getFrequencyText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(getFrequency()));   
  strcat_P(ReturnChar,(PGM_P)F("Hz"));  
  return ReturnChar;      
}

char * PowerSensor::getPowerFactorText(){
  return toText(getPowerFactor());      
}
 
