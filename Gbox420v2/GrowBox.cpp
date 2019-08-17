#include "GrowBox.h"

static char Logs[LogDepth][MaxTextLength];  //two dimensional array for storing log histroy displayed on the website (array of char arrays)

GrowBox::GrowBox(Settings *BoxSettings){ //Constructor
  this -> BoxSettings = BoxSettings;
  Buzzer1 = new Buzzer(BoxSettings -> Buzzer1Pin,&BoxSettings -> Buzzer1Enabled);
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
  Buzzer1 -> refresh();
}

void GrowBox::runFiveSec(){
  InternalDHTSensor -> refresh();
  ExternalDHTSensor -> refresh();
  LightSensor1 -> refresh();
  reportToSerials();  //Logs sensor readings to Serial
}

void GrowBox::runMinute(){
  Light1 -> refresh();
}

void GrowBox::runHalfHour(){    
}

char * GrowBox::reportToSerials(){
  memset(&Message[0], 0, sizeof(Message));  //clear variable 
  strcat(Message,getFormattedTime());
  strcat_P(Message,(PGM_P)F("\n\r DHT Sensor - ")); 
  strcat_P(Message,(PGM_P)F("InternalTemp:")); strcat(Message,InternalDHTSensor -> getTempText());
  strcat_P(Message,(PGM_P)F(" ; InternalHumidity:")); strcat(Message,InternalDHTSensor -> getHumidityText());
  strcat_P(Message,(PGM_P)F(" ; ExternalTemp:")); strcat(Message,ExternalDHTSensor -> getTempText());
  strcat_P(Message,(PGM_P)F(" ; ExternalHumidity:")); strcat(Message,ExternalDHTSensor -> getHumidityText());
//  strcat_P(Message,(PGM_P)F(" ; Internal fan:"));strcat_P(Message,(PGM_P)fanSpeedToText(true));
//  strcat_P(Message,(PGM_P)F(" ; Exhaust fan:"));strcat_P(Message,(PGM_P)fanSpeedToText(false));
//  strcat_P(Message,(PGM_P)F("\n\r Power - "));
//  strcat_P(Message,(PGM_P)F("Power:")); strcat(Message,toText(Power)); strcat_P(Message,(PGM_P)F("W")); 
//  strcat_P(Message,(PGM_P)F(" ; Total:")); strcat(Message,toText(Energy)); strcat_P(Message,(PGM_P)F("Wh"));   
//  strcat_P(Message,(PGM_P)F(" ; Voltage:")); strcat(Message,toText(Voltage)); strcat_P(Message,(PGM_P)F("V"));
//  strcat_P(Message,(PGM_P)F(" ; Current:")); strcat(Message,toText(Current)); strcat_P(Message,(PGM_P)F("A"));
  strcat_P(Message,(PGM_P)F("\n\r Light Sensor - "));
  strcat_P(Message,(PGM_P)F(" ; Brightness:")); strcat(Message,toText(MySettings.Light1Brightness));
  strcat_P(Message,(PGM_P)F(" ; LightReading:")); strcat(Message,LightSensor1 -> getReadingText());
  strcat_P(Message,(PGM_P)F(" (")); strcat(Message,LightSensor1 -> getReadingPercentage());strcat_P(Message,(PGM_P)F(")"));
  strcat_P(Message,(PGM_P)F("\n\r Lights - "));
  strcat_P(Message,(PGM_P)F("Status:")); strcat_P(Message,(PGM_P)Light1 -> getStatusText()); 
  strcat_P(Message,(PGM_P)F(" ; Light detected:")); strcat_P(Message,(PGM_P)LightSensor1 -> getIsDarkText()); 
  strcat_P(Message,(PGM_P)F(" ; LightON:")); strcat(Message,Light1 -> getOnTimeText());
  strcat_P(Message,(PGM_P)F(" ; LightOFF:")); strcat(Message,Light1 -> getOffTimeText());
/*  strcat_P(Message,(PGM_P)F("\n\r Aeroponics - "));
  strcat_P(Message,(PGM_P)F("Pressure:"));strcat(Message,toText(AeroPressure));if(MySettings.MetricSystemEnabled)strcat_P(Message,(PGM_P)F("bar"));else strcat_P(Message,(PGM_P)F("psi"));
  strcat_P(Message,(PGM_P)F(" ; Low:"));strcat(Message,toText(MySettings.AeroPressureLow));
  strcat_P(Message,(PGM_P)F(" ; High:"));strcat(Message,toText(MySettings.AeroPressureHigh));
  strcat_P(Message,(PGM_P)F(" ; Interval:"));strcat(Message,toText(MySettings.AeroInterval));
  strcat_P(Message,(PGM_P)F(" ; Duration:"));strcat(Message,toText(MySettings.AeroDuration));
  strcat_P(Message,(PGM_P)F("\n\r Reservoir - "));
  strcat_P(Message,(PGM_P)F("Temp:")); strcat(Message,toText(ReservoirTemp));  
  strcat_P(Message,(PGM_P)F(" ; PH:")); strcat(Message,toText(PH));
  strcat_P(Message,(PGM_P)F("(")); strcat(Message,toText(PHRaw));strcat_P(Message,(PGM_P)F(")"));
  strcat_P(Message,(PGM_P)F(" ; Reservoir:")); strcat(Message,ReservoirText);
 */
  logToSerials( &Message, true);
}
