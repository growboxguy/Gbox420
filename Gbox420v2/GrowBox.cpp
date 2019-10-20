#include "GrowBox.h"
#include "DHTSensor.h"
#include "Lights.h"
#include "Sound.h"
#include "PowerSensor.h"
#include "LightSensor.h"
#include "PHSensor.h" 
#include "LightSensor.h"
#include "PHSensor.h" 
#include "Aeroponics_Tank.h" 
#include "Aeroponics_NoTank.h" 


static char Logs[LogDepth][MaxTextLength];  //two dimensional array for storing log histroy displayed on the website (array of char arrays)

GrowBox::GrowBox(Settings *BoxSettings){ //Constructor
  this -> BoxSettings = BoxSettings;
  Sound1 = new Sound(F("Sound1"), this, BoxSettings);
  InternalDHTSensor = new DHTSensor(F("InternalDHTSensor"), this, &BoxSettings -> InternalDHTSensor,DHT22);  //passing: Pin and Sensor type: DHT22 or DHT11)
  ExternalDHTSensor = new DHTSensor(F("ExternalDHTSensor"), this, &BoxSettings -> ExternalDHTSensor,DHT22);  //passing: Pin and Sensor type: DHT22 or DHT11)
  LightSensor1 = new LightSensor(F("LightSensor1"), this, &BoxSettings -> LightSensor1);
  Light1 = new Lights(F("Light1"), this, &BoxSettings -> Light1);   //Passing BoxSettings members as references: Changes get written back to BoxSettings and saved to EEPROM. (byte *)(((byte *)&BoxSettings) + offsetof(Settings, LightOnHour))
  PowerSensor1 = new PowerSensor(F("PowerSensor1"), this, &Serial2);
  //Aeroponics_Tank1 = new Aeroponics_Tank(F("Aeroponics_Tank1"), this, &BoxSettings ->Aeroponics_Tank1_Common, &BoxSettings -> Aeroponics_Tank1_Specific);
  Aeroponics_NoTank1 = new Aeroponics_NoTank(F("Aeroponics_NoTank1"), this, &BoxSettings -> Aeroponics_NoTank1_Common, &BoxSettings -> Aeroponics_NoTank1_Specific);
  //PHSensor1 = new PHSensor(this, BoxSettings -> PHSensorInPin,);
  logToSerials(F("GrowBox object created"), true,2);
  addToLog(F("GrowBox initialized"),0);
}

void GrowBox::refresh(){  //implementing the virtual refresh function from Common
 logToSerials(F("GrowBox refreshing"),true,0);
    //trigger all threads at startup
  runFiveSec(); //needs to run first to get sensor readings
  runSec();  
  runMinute();
  runHalfHour();
}

void GrowBox::runSec(){ 
  //if(DebugEnabled)logToSerials(F("One sec trigger.."),true,1);
  Sound1 -> refresh();
  Aeroponics_NoTank1 -> refresh();
}

void GrowBox::runFiveSec(){
  //if(DebugEnabled)logToSerials(F("Five sec trigger.."),true,1); 
  Aeroponics_NoTank1 -> report();  //TODO: Still needs refresh linked into interrupt handling 
  //Aeroponics_Tank1 -> refresh(); //TODO: Still needs refresh linked into interrupt handling 

  //PHSensor1 -> refresh();
}

void GrowBox::runMinute(){
  if(DebugEnabled)logToSerials(F("Minute trigger.."),true,1);
  // Aeroponics_NoTank1 -> refresh();  //TODO: Still needs refresh linked into interrupt handling 
  // Aeroponics_Tank1 -> refresh(); //TODO: Still needs refresh linked into interrupt handling 
  // Light1 -> refresh();
  report();  //Growbox reporting
  InternalDHTSensor -> refresh();
  ExternalDHTSensor -> refresh();
  LightSensor1 -> refresh();
  PowerSensor1 -> refresh();
  Light1 -> refresh();

}

void GrowBox::runHalfHour(){   
  if(DebugEnabled)logToSerials(F("Half hour trigger.."),true,1); 
}

void GrowBox::report(){
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable 
  strcat(LongMessage,getFormattedTime());
  /*
  
  
  strcat_P(LongMessage,(PGM_P)F("\n\r Reservoir - "));
  strcat_P(LongMessage,(PGM_P)F("Temp:")); strcat(LongMessage,toText(ReservoirTemp));  
  strcat_P(LongMessage,(PGM_P)F(" ; PH:")); strcat(LongMessage,toText(PH));
  strcat_P(LongMessage,(PGM_P)F("(")); strcat(LongMessage,toText(PHRaw));strcat_P(LongMessage,(PGM_P)F(")"));
  strcat_P(LongMessage,(PGM_P)F(" ; Reservoir:")); strcat(LongMessage,ReservoirText);
 */
  logToSerials( &LongMessage, true,0);
}

void GrowBox::addToLog(const char *LongMessage,byte Indent){  //adds a log entry that is displayed on the web interface
  logToSerials(LongMessage,true,Indent);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],MaxTextLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy(Logs[0],LongMessage,MaxTextLength);  //instert new log to [0]
}

void GrowBox::addToLog(const __FlashStringHelper *LongMessage,byte Indent){ //function overloading: same function name, different parameter type 
  logToSerials(LongMessage,true,Indent);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],MaxTextLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy_P(Logs[0],(PGM_P)LongMessage,MaxTextLength);  //instert new log to [0]
}

char* GrowBox::eventLogToJSON(bool Append){ //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  if(!Append)memset(&LongMessage[0], 0, sizeof(LongMessage));
  strcat_P(LongMessage,(PGM_P)F("["));
  for(int i=LogDepth-1;i >=0 ; i--)
  {
   strcat_P(LongMessage,(PGM_P)F("\""));
   strcat(LongMessage,Logs[i]);
   strcat_P(LongMessage,(PGM_P)F("\""));
   if(i > 0 ) strcat_P(LongMessage,(PGM_P)F(","));
  }
  LongMessage[strlen(LongMessage)] = ']';
  return LongMessage;
}

void GrowBox::setDebugOnOff(bool State){
  BoxSettings -> DebugEnabled = State;
  if(BoxSettings -> DebugEnabled){ 
    addToLog(F("Debug LongMessages enabled"));
    Sound1 -> playOnSound();
    }
  else {
    addToLog(F("Debug LongMessages disabled"));
    Sound1 -> playOffSound();
    }
}

void GrowBox::setMetricSystemEnabled(bool MetricEnabled){ 
  if(MetricEnabled != BoxSettings -> MetricSystemEnabled){  //if there was a change
    BoxSettings -> MetricSystemEnabled = MetricEnabled;
    BoxSettings -> InternalFanSwitchTemp = convertBetweenTempUnits(BoxSettings -> InternalFanSwitchTemp);
    BoxSettings -> InternalDHTSensor.TempAlertLow= convertBetweenTempUnits(BoxSettings -> InternalDHTSensor.TempAlertLow);
    BoxSettings -> InternalDHTSensor.TempAlertHigh= convertBetweenTempUnits(BoxSettings -> InternalDHTSensor.TempAlertHigh);
    BoxSettings -> ExternalDHTSensor.TempAlertLow= convertBetweenTempUnits(BoxSettings -> ExternalDHTSensor.TempAlertLow);
    BoxSettings -> ExternalDHTSensor.TempAlertHigh= convertBetweenTempUnits(BoxSettings -> ExternalDHTSensor.TempAlertHigh);
    BoxSettings -> PressureSensor1.PressureAlertLow=convertBetweenPressureUnits(BoxSettings -> PressureSensor1.PressureAlertLow);
    BoxSettings -> PressureSensor1.PressureAlertHigh=convertBetweenPressureUnits(BoxSettings -> PressureSensor1.PressureAlertHigh);
  }    
  if(BoxSettings -> MetricSystemEnabled) addToLog(F("Using Metric units"));
  else addToLog(F("Using Imperial units"));  
}  
