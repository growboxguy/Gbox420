void ReportToGoogleSheets(bool AddToLog){
  if(AddToLog)addToLog(F("Reporting to Google Sheets"));
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat_P(WebMessage,(PGM_P)F("/pushingbox?devid=")); strcat(WebMessage,MySettings.PushingBoxLogRelayID);
  strcat_P(WebMessage,(PGM_P)F("&Log="));logToJSON(false,true);  
  logToSerials(F("Reporting to Google Sheets: "),false); logToSerials(WebMessage,true);   
  RestAPI.get(WebMessage);
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
  if(!Append)memset(&WebMessage[0], 0, sizeof(WebMessage));
  strcat_P(WebMessage,(PGM_P)F("["));
  for(int i=LogDepth-1;i >=0 ; i--)
  {
   strcat_P(WebMessage,(PGM_P)F("\""));
   strcat(WebMessage,Logs[i]);
   strcat_P(WebMessage,(PGM_P)F("\""));
   if(i > 0 ) strcat_P(WebMessage,(PGM_P)F(","));
  }
  WebMessage[strlen(WebMessage)] = ']';
  return WebMessage;
}

char* logToJSON(bool AddToLog,bool Append){ //publish readings in JSON format
  if(AddToLog)addToLog(F("Reporting to MQTT"));
  if(!Append)memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat_P(WebMessage,(PGM_P)F("{\"BoxDate\":\""));  strcat(WebMessage,getFormattedTime());
  strcat_P(WebMessage,(PGM_P)F("\",\"InternalTemp\":\""));  strcat(WebMessage,toText(InternalTemp));
  strcat_P(WebMessage,(PGM_P)F("\",\"ExternalTemp\":\""));  strcat(WebMessage,toText(ExternalTemp));
  strcat_P(WebMessage,(PGM_P)F("\",\"InternalHumidity\":\""));  strcat(WebMessage,toText(InternalHumidity));
  strcat_P(WebMessage,(PGM_P)F("\",\"ExternalHumidity\":\""));  strcat(WebMessage,toText(ExternalHumidity));
  strcat_P(WebMessage,(PGM_P)F("\",\"InternalFan\":\"")); strcat_P(WebMessage,(PGM_P)fanSpeedToNumber(true));
  strcat_P(WebMessage,(PGM_P)F("\",\"ExhaustFan\":\"")); strcat_P(WebMessage,(PGM_P)fanSpeedToNumber(false));
  strcat_P(WebMessage,(PGM_P)F("\",\"Power\":\""));  strcat(WebMessage,toText(Power)); 
  strcat_P(WebMessage,(PGM_P)F("\",\"Energy\":\""));  strcat(WebMessage,toText(Energy));
  strcat_P(WebMessage,(PGM_P)F("\",\"Voltage\":\""));  strcat(WebMessage,toText(Voltage));
  strcat_P(WebMessage,(PGM_P)F("\",\"Current\":\""));  strcat(WebMessage,toText(Current));
  strcat_P(WebMessage,(PGM_P)F("\",\"Lights\":\""));  strcat(WebMessage,toText(MySettings.LightStatus));
  strcat_P(WebMessage,(PGM_P)F("\",\"Brightness\":\""));  strcat(WebMessage,toText(MySettings.LightBrightness));
  strcat_P(WebMessage,(PGM_P)F("\",\"LightReading\":\""));  strcat(WebMessage,toText(LightReading));
  strcat_P(WebMessage,(PGM_P)F("\",\"Bright\":\""));  strcat(WebMessage,toText(Bright));
  strcat_P(WebMessage,(PGM_P)F("\",\"Reservoir\":\""));  strcat(WebMessage,toText(ReservoirLevel));
  strcat_P(WebMessage,(PGM_P)F("\",\"PH\":\""));  strcat(WebMessage,toText(PH));
  strcat_P(WebMessage,(PGM_P)F("\",\"ReservoirTemp\":\""));  strcat(WebMessage,toText(ReservoirTemp));
  strcat_P(WebMessage,(PGM_P)F("\",\"Pressure\":\""));  strcat(WebMessage,toText(AeroPressure));
  strcat_P(WebMessage,(PGM_P)F("\",\"AeroInterval\":\"")); strcat(WebMessage,toText(MySettings.AeroInterval));
  strcat_P(WebMessage,(PGM_P)F("\",\"AeroDuration\":\"")); strcat(WebMessage,toText(MySettings.AeroDuration));
  strcat_P(WebMessage,(PGM_P)F("\"}"));
  return WebMessage;
}

char * logToText(){
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable 
  strcat(WebMessage,getFormattedTime());
  strcat_P(WebMessage,(PGM_P)F("\n\r Box - ")); 
  strcat_P(WebMessage,(PGM_P)F("InternalTemp:")); strcat(WebMessage,toText(InternalTemp)); if(MySettings.MetricSystemEnabled)strcat_P(WebMessage,(PGM_P)F("C")); else strcat_P(WebMessage,(PGM_P)F("F"));
  strcat_P(WebMessage,(PGM_P)F(" ; InternalHumidity:")); strcat(WebMessage,toText(InternalHumidity)); strcat_P(WebMessage,(PGM_P)F("%"));
  strcat_P(WebMessage,(PGM_P)F(" ; ExternalTemp:")); strcat(WebMessage,toText(ExternalTemp)); if(MySettings.MetricSystemEnabled)strcat_P(WebMessage,(PGM_P)F("C")); else strcat_P(WebMessage,(PGM_P)F("F"));
  strcat_P(WebMessage,(PGM_P)F(" ; ExternalHumidity:")); strcat(WebMessage,toText(ExternalHumidity)); strcat_P(WebMessage,(PGM_P)F("%"));
  strcat_P(WebMessage,(PGM_P)F(" ; Internal fan:"));strcat_P(WebMessage,(PGM_P)fanSpeedToText(true));
  strcat_P(WebMessage,(PGM_P)F(" ; Exhaust fan:"));strcat_P(WebMessage,(PGM_P)fanSpeedToText(false));
  strcat_P(WebMessage,(PGM_P)F("\n\r Power - "));
  strcat_P(WebMessage,(PGM_P)F("Power:")); strcat(WebMessage,toText(Power)); strcat_P(WebMessage,(PGM_P)F("W")); 
  strcat_P(WebMessage,(PGM_P)F(" ; Total:")); strcat(WebMessage,toText(Energy)); strcat_P(WebMessage,(PGM_P)F("Wh"));   
  strcat_P(WebMessage,(PGM_P)F(" ; Voltage:")); strcat(WebMessage,toText(Voltage)); strcat_P(WebMessage,(PGM_P)F("V"));
  strcat_P(WebMessage,(PGM_P)F(" ; Current:")); strcat(WebMessage,toText(Current)); strcat_P(WebMessage,(PGM_P)F("A"));
  strcat_P(WebMessage,(PGM_P)F("\n\r Lights - "));
  strcat_P(WebMessage,(PGM_P)F("Light:")); strcat_P(WebMessage,(PGM_P)stateToText(MySettings.LightStatus)); 
  strcat_P(WebMessage,(PGM_P)F(" ; Brightness:")); strcat(WebMessage,toText(MySettings.LightBrightness));
  strcat_P(WebMessage,(PGM_P)F(" ; LightReading:")); strcat(WebMessage,toText(LightReading));
  strcat_P(WebMessage,(PGM_P)F(" ; Light detected:")); strcat_P(WebMessage,(PGM_P)BrightToText()); 
  strcat_P(WebMessage,(PGM_P)F(" ; LightON:")); strcat(WebMessage,timetoText(MySettings.LightOnHour, MySettings.LightOnMinute));
  strcat_P(WebMessage,(PGM_P)F(" ; LightOFF:")); strcat(WebMessage,timetoText(MySettings.LightOffHour, MySettings.LightOffMinute));
  strcat_P(WebMessage,(PGM_P)F("\n\r Aeroponics - "));
  strcat_P(WebMessage,(PGM_P)F("Pressure:"));strcat(WebMessage,toText(AeroPressure));if(MySettings.MetricSystemEnabled)strcat_P(WebMessage,(PGM_P)F("bar"));else strcat_P(WebMessage,(PGM_P)F("psi"));
  strcat_P(WebMessage,(PGM_P)F(" ; Low:"));strcat(WebMessage,toText(MySettings.AeroLowPressure));
  strcat_P(WebMessage,(PGM_P)F(" ; High:"));strcat(WebMessage,toText(MySettings.AeroHighPressure));
  strcat_P(WebMessage,(PGM_P)F(" ; Interval:"));strcat(WebMessage,toText(MySettings.AeroInterval));
  strcat_P(WebMessage,(PGM_P)F(" ; Duration:"));strcat(WebMessage,toText(MySettings.AeroDuration));
  strcat_P(WebMessage,(PGM_P)F("\n\r Reservoir - "));
  strcat_P(WebMessage,(PGM_P)F("Temp:")); strcat(WebMessage,toText(ReservoirTemp));  
  strcat_P(WebMessage,(PGM_P)F(" ; PH:")); strcat(WebMessage,toText(PH));
  strcat_P(WebMessage,(PGM_P)F("(")); strcat(WebMessage,toText(PHRaw));strcat_P(WebMessage,(PGM_P)F(")"));
  strcat_P(WebMessage,(PGM_P)F(" ; Reservoir:")); strcat(WebMessage,ReservoirText);
  return WebMessage;
}

char * toText(int Number){
  static char ReturnChar[MaxTextLength] = "";
  itoa(Number, ReturnChar, 10);
  return ReturnChar;
}

char * toText(uint32_t Number){
  static char ReturnChar[MaxTextLength] = "";
  itoa(Number, ReturnChar, 10);
  return ReturnChar;
}

char * toText(int Number1, int Number2,const char * Separator){  //function overloading: Same named function, different parameters
  static char ReturnChar[MaxTextLength] = ""; 
  snprintf(ReturnChar,MaxTextLength,"%d%s%d",Number1,Separator,Number2);
  return ReturnChar;
}

char * toText(float Number){ 
  static char ReturnFloatChar[MaxTextLength] = ""; 
  if(isnan(Number)) Number= -1.0;
  dtostrf(Number, 4, 2, ReturnFloatChar); //minimum 4 char total lengt, with 2 decimals
  return ReturnFloatChar;
}

char * toPrecisionText(float Number){ 
  static char ReturnFloatChar[MaxTextLength] = "";
  if(isnan(Number)) Number= -1.0;
  dtostrf(Number, 8, 6, ReturnFloatChar); //minimum 8 char total lengt, with 6 decimals
  return ReturnFloatChar;
}

char * toText(float Number1, float Number2,const char * Separator){
  static char ReturnChar[MaxTextLength] = ""; 
  static char Number2Char[MaxTextLength] = "";
  if(isnan(Number1)) Number1= -1.0;
  if(isnan(Number2)) Number2= -1.0;
  dtostrf(Number1, 4, 2, ReturnChar); 
  strcat(ReturnChar,Separator);
  dtostrf(Number2, 4, 2, Number2Char); 
  strcat(ReturnChar,Number2Char);
  return ReturnChar;
}

char * timetoText(int Hour, int Minute){
  static char ReturnChar[MaxTextLength] = ""; //2 digit + separator + 2 digit + null
  sprintf (ReturnChar, "%02u:%02u", Hour, Minute);
  return ReturnChar;
}

const __FlashStringHelper * stateToText(bool Status){
   if(Status) return F("ON");
   else return F("OFF");
} 

const __FlashStringHelper * statusToText(bool Status){
   if(Status) return F("OK");
   else return F("!!!");
} 

void setDebug(bool State){
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
