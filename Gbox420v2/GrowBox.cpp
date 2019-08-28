#include "GrowBox.h"
#include "420Pins.h"  //Arduino Pin layout
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
  Sound1 = new Sound(F("Sound1"), this, Sound1Pin, &BoxSettings -> Sound1Enabled);
  InternalDHTSensor = new DHTSensor(F("InternalDHTSensor"), this, InternalDHTSensorPin,DHT22);  //passing: Pin and Sensor type: DHT22 or DHT11)
  ExternalDHTSensor = new DHTSensor(F("ExternalDHTSensor"), this, ExternalDHTSensorPin,DHT22);  //passing: Pin and Sensor type: DHT22 or DHT11)
  LightSensor1 = new LightSensor(F("LightSensor1"), this, LightSensor1DigitalPin, LightSensor1AnalogPin);
  Light1 = new Lights(F("Light1"), this,Light1RelayPin, Light1DimmingPin, &BoxSettings -> Light1, 8);   //Passing BoxSettings members as references: Changes get written back to BoxSettings and saved to EEPROM. (byte *)(((byte *)&BoxSettings) + offsetof(Settings, LightOnHour))
  PowerSensor1 = new PowerSensor(F("PowerSensor1"), this, &Serial2);
  Aeroponics_Tank1 = new Aeroponics_Tank(F("Aeroponics_Tank1"), this, AeroSpraySolenoidPin, AeroBypassSolenoidPin, AeroPumpPin, &BoxSettings ->Aeroponics_Tank1, &BoxSettings -> Aeroponics_Tank1_TankSpecific);
  Aeroponics_NoTank1 = new Aeroponics_NoTank(F("Aeroponics_NoTank1"), this, AeroBypassSolenoidPin, AeroPumpPin, &BoxSettings -> Aeroponics_NoTank1);
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
  if(DebugEnabled)logToSerials(F("One sec trigger.."),true,1);
  Sound1 -> refresh();
}

void GrowBox::runFiveSec(){
  if(DebugEnabled)logToSerials(F("Five sec trigger.."),true,1); 
  report();  //Growbox reporting
  InternalDHTSensor -> refresh();
  ExternalDHTSensor -> refresh();
  LightSensor1 -> refresh();
  PowerSensor1 -> refresh();
  Aeroponics_NoTank1 -> refresh();  //TODO: Still needs refresh linked into interrupt handling 
  Aeroponics_Tank1 -> refresh(); //TODO: Still needs refresh linked into interrupt handling 
  Light1 -> refresh();
  //PHSensor1 -> refresh();
}

void GrowBox::runMinute(){
  if(DebugEnabled)logToSerials(F("Minute trigger.."),true,1);
  // Aeroponics_NoTank1 -> refresh();  //TODO: Still needs refresh linked into interrupt handling 
  // Aeroponics_Tank1 -> refresh(); //TODO: Still needs refresh linked into interrupt handling 
  // Light1 -> refresh();

}

void GrowBox::runHalfHour(){   
  if(DebugEnabled)logToSerials(F("Half hour trigger.."),true,1); 
}

void GrowBox::report(){
  memset(&Message[0], 0, sizeof(Message));  //clear variable 
  strcat(Message,getFormattedTime());
  /*
  
  
  strcat_P(Message,(PGM_P)F("\n\r Reservoir - "));
  strcat_P(Message,(PGM_P)F("Temp:")); strcat(Message,toText(ReservoirTemp));  
  strcat_P(Message,(PGM_P)F(" ; PH:")); strcat(Message,toText(PH));
  strcat_P(Message,(PGM_P)F("(")); strcat(Message,toText(PHRaw));strcat_P(Message,(PGM_P)F(")"));
  strcat_P(Message,(PGM_P)F(" ; Reservoir:")); strcat(Message,ReservoirText);
 */
  logToSerials( &Message, true,0);
}

void GrowBox::addToLog(const char *Message,byte Indent){  //adds a log entry that is displayed on the web interface
  logToSerials(Message,true,Indent);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],MaxTextLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy(Logs[0],Message,MaxTextLength);  //instert new log to [0]
}

void GrowBox::addToLog(const __FlashStringHelper *Message,byte Indent){ //function overloading: same function name, different parameter type 
  logToSerials(Message,true,Indent);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],MaxTextLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy_P(Logs[0],(PGM_P)Message,MaxTextLength);  //instert new log to [0]
}

char* GrowBox::eventLogToJSON(bool Append){ //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  if(!Append)memset(&Message[0], 0, sizeof(Message));
  strcat_P(Message,(PGM_P)F("["));
  for(int i=LogDepth-1;i >=0 ; i--)
  {
   strcat_P(Message,(PGM_P)F("\""));
   strcat(Message,Logs[i]);
   strcat_P(Message,(PGM_P)F("\""));
   if(i > 0 ) strcat_P(Message,(PGM_P)F(","));
  }
  Message[strlen(Message)] = ']';
  return Message;
}

void GrowBox::setDebugOnOff(bool State){
  BoxSettings -> DebugEnabled = State;
  if(BoxSettings -> DebugEnabled){ 
    addToLog(F("Debug messages enabled"));
    Sound1 -> playOnSound();
    }
  else {
    addToLog(F("Debug messages disabled"));
    Sound1 -> playOffSound();
    }
}

void GrowBox::setMetricSystemEnabled(bool MetricEnabled){ 
  if(MetricEnabled != BoxSettings -> MetricSystemEnabled){  //if there was a change
    BoxSettings -> MetricSystemEnabled = MetricEnabled;
    BoxSettings -> InternalFanSwitchTemp = convertBetweenTempUnits(BoxSettings -> InternalFanSwitchTemp);
    BoxSettings -> TempAlertLow= convertBetweenTempUnits(BoxSettings -> TempAlertLow);
    BoxSettings -> TempAlertHigh= convertBetweenTempUnits(BoxSettings -> TempAlertHigh);
    BoxSettings -> PressureAlertLow=convertBetweenPressureUnits(BoxSettings -> PressureAlertLow);
    BoxSettings -> PressureAlertHigh=convertBetweenPressureUnits(BoxSettings -> PressureAlertHigh);
  }    
  if(BoxSettings -> MetricSystemEnabled) addToLog(F("Using Metric units"));
  else addToLog(F("Using Imperial units"));  
}  
