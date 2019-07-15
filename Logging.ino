void ReportToGoogleSheets(bool AddToLog){
  if(AddToLog)addToLog(F("Reporting to Google Sheets"));
  memset(&Message[0], 0, sizeof(Message));  //clear variable
  strcat_P(Message,(PGM_P)F("/pushingbox?devid=")); strcat(Message,MySettings.PushingBoxLogRelayID);
  strcat_P(Message,(PGM_P)F("&Log="));logToJSON(false,true);  
  logToSerials(F("Reporting to Google Sheets: "),false); logToSerials(Message,true);   
  RestAPI.get(Message);
}

template <class logLine>   //fuction template: logToSerials can take any parameter type (int,float,bool,char..) https://en.cppreference.com/w/cpp/language/function_template
void logToSerials (logLine ToPrint,bool breakLine) {
 if(breakLine){
    Serial.println(ToPrint);
    Serial3.println(ToPrint);
  }
  else{
    Serial.print(ToPrint);
    Serial3.print(ToPrint);
  }
}

void addToLog(const char *message){
  logToSerials(message,true);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],MaxTextLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy(Logs[0],message,MaxTextLength);  //instert new log to [0]
}

void addToLog(const __FlashStringHelper *message){ //function overloading: same function name, different parameter type 
  logToSerials(message,true);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],MaxTextLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy_P(Logs[0],(PGM_P)message,MaxTextLength);  //instert new log to [0]
}

char* eventLogToJSON(bool Append){ //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
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

char* logToJSON(bool AddToLog,bool Append){ //publish readings in JSON format
  if(AddToLog)addToLog(F("Reporting to MQTT"));
  if(!Append)memset(&Message[0], 0, sizeof(Message));  //clear variable
  strcat_P(Message,(PGM_P)F("{\"BoxDate\":\""));  strcat(Message,getFormattedTime());
  strcat_P(Message,(PGM_P)F("\",\"InternalTemp\":\""));  strcat(Message,toText(InternalTemp));
  strcat_P(Message,(PGM_P)F("\",\"ExternalTemp\":\""));  strcat(Message,toText(ExternalTemp));
  strcat_P(Message,(PGM_P)F("\",\"InternalHumidity\":\""));  strcat(Message,toText(InternalHumidity));
  strcat_P(Message,(PGM_P)F("\",\"ExternalHumidity\":\""));  strcat(Message,toText(ExternalHumidity));
  strcat_P(Message,(PGM_P)F("\",\"InternalFan\":\"")); strcat_P(Message,(PGM_P)fanSpeedToNumber(true));
  strcat_P(Message,(PGM_P)F("\",\"ExhaustFan\":\"")); strcat_P(Message,(PGM_P)fanSpeedToNumber(false));
  strcat_P(Message,(PGM_P)F("\",\"Power\":\""));  strcat(Message,toText(Power)); 
  strcat_P(Message,(PGM_P)F("\",\"Energy\":\""));  strcat(Message,toText(Energy));
  strcat_P(Message,(PGM_P)F("\",\"Voltage\":\""));  strcat(Message,toText(Voltage));
  strcat_P(Message,(PGM_P)F("\",\"Current\":\""));  strcat(Message,toText(Current));
  strcat_P(Message,(PGM_P)F("\",\"Lights\":\""));  strcat(Message,toText(MySettings.LightStatus));
  strcat_P(Message,(PGM_P)F("\",\"Brightness\":\""));  strcat(Message,toText(MySettings.LightBrightness));
  strcat_P(Message,(PGM_P)F("\",\"LightReading\":\""));  strcat(Message,toText(LightReading));
  strcat_P(Message,(PGM_P)F("\",\"Bright\":\""));  strcat(Message,toText(Bright));
  strcat_P(Message,(PGM_P)F("\",\"Reservoir\":\""));  strcat(Message,toText(ReservoirLevel));
  strcat_P(Message,(PGM_P)F("\",\"PH\":\""));  strcat(Message,toText(PH));
  strcat_P(Message,(PGM_P)F("\",\"ReservoirTemp\":\""));  strcat(Message,toText(ReservoirTemp));
  strcat_P(Message,(PGM_P)F("\",\"Pressure\":\""));  strcat(Message,toText(AeroPressure));
  strcat_P(Message,(PGM_P)F("\",\"AeroInterval\":\"")); strcat(Message,toText(MySettings.AeroInterval));
  strcat_P(Message,(PGM_P)F("\",\"AeroDuration\":\"")); strcat(Message,toText(MySettings.AeroDuration));
  strcat_P(Message,(PGM_P)F("\"}"));
  return Message;
}

char * logToText(){
  memset(&Message[0], 0, sizeof(Message));  //clear variable 
  strcat(Message,getFormattedTime());
  strcat_P(Message,(PGM_P)F("\n\r Box - ")); 
  strcat_P(Message,(PGM_P)F("InternalTemp:")); strcat(Message,toText(InternalTemp)); if(MySettings.MetricSystemEnabled)strcat_P(Message,(PGM_P)F("C")); else strcat_P(Message,(PGM_P)F("F"));
  strcat_P(Message,(PGM_P)F(" ; InternalHumidity:")); strcat(Message,toText(InternalHumidity)); strcat_P(Message,(PGM_P)F("%"));
  strcat_P(Message,(PGM_P)F(" ; ExternalTemp:")); strcat(Message,toText(ExternalTemp)); if(MySettings.MetricSystemEnabled)strcat_P(Message,(PGM_P)F("C")); else strcat_P(Message,(PGM_P)F("F"));
  strcat_P(Message,(PGM_P)F(" ; ExternalHumidity:")); strcat(Message,toText(ExternalHumidity)); strcat_P(Message,(PGM_P)F("%"));
  strcat_P(Message,(PGM_P)F(" ; Internal fan:"));strcat_P(Message,(PGM_P)fanSpeedToText(true));
  strcat_P(Message,(PGM_P)F(" ; Exhaust fan:"));strcat_P(Message,(PGM_P)fanSpeedToText(false));
  strcat_P(Message,(PGM_P)F("\n\r Power - "));
  strcat_P(Message,(PGM_P)F("Power:")); strcat(Message,toText(Power)); strcat_P(Message,(PGM_P)F("W")); 
  strcat_P(Message,(PGM_P)F(" ; Total:")); strcat(Message,toText(Energy)); strcat_P(Message,(PGM_P)F("Wh"));   
  strcat_P(Message,(PGM_P)F(" ; Voltage:")); strcat(Message,toText(Voltage)); strcat_P(Message,(PGM_P)F("V"));
  strcat_P(Message,(PGM_P)F(" ; Current:")); strcat(Message,toText(Current)); strcat_P(Message,(PGM_P)F("A"));
  strcat_P(Message,(PGM_P)F("\n\r Lights - "));
  strcat_P(Message,(PGM_P)F("Light:")); strcat_P(Message,(PGM_P)onOffToText(MySettings.LightStatus)); 
  strcat_P(Message,(PGM_P)F(" ; Brightness:")); strcat(Message,toText(MySettings.LightBrightness));
  strcat_P(Message,(PGM_P)F(" ; LightReading:")); strcat(Message,toText(LightReading));
  strcat_P(Message,(PGM_P)F(" ; Light detected:")); strcat_P(Message,(PGM_P)yesNoToText()); 
  strcat_P(Message,(PGM_P)F(" ; LightON:")); strcat(Message,timetoText(MySettings.LightOnHour, MySettings.LightOnMinute));
  strcat_P(Message,(PGM_P)F(" ; LightOFF:")); strcat(Message,timetoText(MySettings.LightOffHour, MySettings.LightOffMinute));
  strcat_P(Message,(PGM_P)F("\n\r Aeroponics - "));
  strcat_P(Message,(PGM_P)F("Pressure:"));strcat(Message,toText(AeroPressure));if(MySettings.MetricSystemEnabled)strcat_P(Message,(PGM_P)F("bar"));else strcat_P(Message,(PGM_P)F("psi"));
  strcat_P(Message,(PGM_P)F(" ; Interval:"));strcat(Message,toText(MySettings.AeroInterval));
  strcat_P(Message,(PGM_P)F(" ; Duration:"));strcat(Message,toText(MySettings.AeroDuration));
  strcat_P(Message,(PGM_P)F("\n\r Reservoir - "));
  strcat_P(Message,(PGM_P)F("Temp:")); strcat(Message,toText(ReservoirTemp));  
  strcat_P(Message,(PGM_P)F(" ; PH:")); strcat(Message,toText(PH));
  strcat_P(Message,(PGM_P)F("(")); strcat(Message,toText(PHRaw));strcat_P(Message,(PGM_P)F(")"));
  strcat_P(Message,(PGM_P)F(" ; Reservoir:")); strcat(Message,ReservoirText);
  return Message;
}

void setDebugOnOff(bool State){
  MySettings.DebugEnabled = State;
  if(MySettings.DebugEnabled){ 
    addToLog(F("Debug enabled"));
    PlayOnSound=true;}
  else {
    addToLog(F("Debug disabled"));
    PlayOffSound=true;
    }
}

void setReportToGoogleSheetsOnOff(bool State){
  MySettings.ReportToGoogleSheets = State;
  if(MySettings.ReportToGoogleSheets){ 
    addToLog(F("Google Sheets enabled"));
    PlayOnSound=true;
    }
  else {
    addToLog(F("Google Sheets disabled"));
    PlayOffSound=true;
    }
}

void setReportToMqttOnOff(bool State){
  MySettings.ReportToMqtt = State;
  if(MySettings.ReportToMqtt){ 
    addToLog(F("MQTT enabled"));
    PlayOnSound=true;
    }
  else {
    addToLog(F("MQTT disabled"));
    PlayOffSound=true;
    }
}

void setPushingBoxLogRelayID(char * ID){  
  strncpy(MySettings.PushingBoxLogRelayID,ID,MaxTextLength);
  addToLog(F("Sheets log relay ID updated")); 
  logToSerials(F("New PushingBox DeviceID: "),false);logToSerials(MySettings.PushingBoxLogRelayID,true); 
}
