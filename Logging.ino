void ReportToGoogleSheets(bool AddToLog){
  if(AddToLog)addToLog(F("Reporting to Google Sheets"));
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat_P(WebMessage,(PGM_P)F("/pushingbox?devid=")); strcat(WebMessage,PushingBoxLogRelayID);
  strcat_P(WebMessage,(PGM_P)F("&BoxDate="));  strcat(WebMessage,CurrentTime);
  strcat_P(WebMessage,(PGM_P)F("&BoxTempC="));  strcat(WebMessage,toText(BoxTempC));
  strcat_P(WebMessage,(PGM_P)F("&BoxTempF="));  strcat(WebMessage,toText(BoxTempF));
  strcat_P(WebMessage,(PGM_P)F("&Humidity="));  strcat(WebMessage,toText(Humidity));
  strcat_P(WebMessage,(PGM_P)F("&Power="));  strcat(WebMessage,toText(Power)); 
  strcat_P(WebMessage,(PGM_P)F("&Energy="));  strcat(WebMessage,toText(Energy));
  strcat_P(WebMessage,(PGM_P)F("&Voltage="));  strcat(WebMessage,toText(Voltage));
  strcat_P(WebMessage,(PGM_P)F("&Current="));  strcat(WebMessage,toText(Current));
  strcat_P(WebMessage,(PGM_P)F("&PH="));  strcat(WebMessage,toText(PH));
  strcat_P(WebMessage,(PGM_P)F("&Moisture="));  strcat(WebMessage,toText(Moisture));
  strcat_P(WebMessage,(PGM_P)F("&isLightOn="));  strcat(WebMessage,toText(MySettings.isLightOn));
  strcat_P(WebMessage,(PGM_P)F("&Brightness="));  strcat(WebMessage,toText(MySettings.LightBrightness));
  strcat_P(WebMessage,(PGM_P)F("&LightReading="));  strcat(WebMessage,toText(LightReading));
  strcat_P(WebMessage,(PGM_P)F("&isBright="));  strcat(WebMessage,toText(isBright));
  strcat_P(WebMessage,(PGM_P)F("&Reservoir: (")); strcat(WebMessage,toText(reservoirPercent)); 
  strcat_P(WebMessage,(PGM_P)F("&InternalFan=")); strcat_P(WebMessage,(PGM_P)internalFanSpeedToText()); //strcat_P is the same as strcat, just for __FlashStringHelper type (stored in flash)
  strcat_P(WebMessage,(PGM_P)F("&ExhaustFan=")); strcat_P(WebMessage,(PGM_P)exhaustFanSpeedToText());
  LogToSerials(F("Reporting to Google Sheets: "),false); LogToSerials(WebMessage,true);   
  RestAPI.get(WebMessage);
}

template <class logLine>   //fuction template: LogToSerials can take any parameter type (int,float,bool,char..) https://en.cppreference.com/w/cpp/language/function_template
void LogToSerials (logLine ToPrint,bool breakLine) {
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
  LogToSerials(message,true);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],LogLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy(Logs[0],message,LogLength);  //instert new log to [0]
}

void addToLog(const __FlashStringHelper *message){ //function overloading: same function name, different parameter type 
  LogToSerials(message,true);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],LogLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy_P(Logs[0],(PGM_P)message,LogLength);  //instert new log to [0]
}

char* eventLogToJSON(){
  memset(&WebMessage[0], 0, sizeof(WebMessage));
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



char * logToText(){
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable 
  strcat(WebMessage,CurrentTime);
  strcat_P(WebMessage,(PGM_P)F("\n\r Box - ")); 
  strcat_P(WebMessage,(PGM_P)F("TempC:")); strcat(WebMessage,toText(BoxTempC)); strcat_P(WebMessage,(PGM_P)F("C"));
  strcat_P(WebMessage,(PGM_P)F(" ; TempF:")); strcat(WebMessage,toText(BoxTempF)); strcat_P(WebMessage,(PGM_P)F("F"));
  strcat_P(WebMessage,(PGM_P)F(" ; Humidity:")); strcat(WebMessage,toText(Humidity)); strcat_P(WebMessage,(PGM_P)F("%"));
  strcat_P(WebMessage,(PGM_P)F(" ; Internal fan:"));strcat_P(WebMessage,(PGM_P)internalFanSpeedToText());
  strcat_P(WebMessage,(PGM_P)F(" ; Exhaust fan:"));strcat_P(WebMessage,(PGM_P)exhaustFanSpeedToText());
  strcat_P(WebMessage,(PGM_P)F("\n\r Power - "));
  strcat_P(WebMessage,(PGM_P)F("12V supply:")); strcat_P(WebMessage,(PGM_P)powerSupplyToText());
  strcat_P(WebMessage,(PGM_P)F(" ; Power:")); strcat(WebMessage,toText(Power)); strcat_P(WebMessage,(PGM_P)F("W")); 
  strcat_P(WebMessage,(PGM_P)F(" ; Total:")); strcat(WebMessage,toText(Energy)); strcat_P(WebMessage,(PGM_P)F("Wh"));   
  strcat_P(WebMessage,(PGM_P)F(" ; Voltage:")); strcat(WebMessage,toText(Voltage)); strcat_P(WebMessage,(PGM_P)F("V"));
  strcat_P(WebMessage,(PGM_P)F(" ; Current:")); strcat(WebMessage,toText(Current)); strcat_P(WebMessage,(PGM_P)F("A"));
  strcat_P(WebMessage,(PGM_P)F("\n\r Lights - "));
  strcat_P(WebMessage,(PGM_P)F("Light:")); strcat_P(WebMessage,(PGM_P)lightStatusToText()); 
  strcat_P(WebMessage,(PGM_P)F(" ; Brightness:")); strcat(WebMessage,toText(MySettings.LightBrightness));
  strcat_P(WebMessage,(PGM_P)F(" ; LightReading:")); strcat(WebMessage,toText(LightReading)); strcat_P(WebMessage,(PGM_P)F(" - "));strcat(WebMessage,toText(LightReadingPercent));  strcat_P(WebMessage,(PGM_P)F("%"));
  strcat_P(WebMessage,(PGM_P)F(" ; Light detected:")); strcat_P(WebMessage,(PGM_P)isBrightToText()); 
  strcat_P(WebMessage,(PGM_P)F(" ; LightON:")); strcat(WebMessage,timetoText(MySettings.LightOnHour, MySettings.LightOnMinute));
  strcat_P(WebMessage,(PGM_P)F(" ; LightOFF:")); strcat(WebMessage,timetoText(MySettings.LightOffHour, MySettings.LightOffMinute));
  strcat_P(WebMessage,(PGM_P)F("\n\r Aeroponics - "));
  strcat_P(WebMessage,(PGM_P)F("Pressure:"));strcat(WebMessage,toText(AeroPressure));strcat_P(WebMessage,(PGM_P)F("bar/"));strcat(WebMessage,toText(AeroPressurePSI));strcat_P(WebMessage,(PGM_P)F("psi"));
  strcat_P(WebMessage,(PGM_P)F(" ; Low:"));strcat(WebMessage,toText(MySettings.AeroPressureLow));
  strcat_P(WebMessage,(PGM_P)F(" ; High:"));strcat(WebMessage,toText(MySettings.AeroPressureHigh));
  strcat_P(WebMessage,(PGM_P)F(" ; PumpState:"));strcat_P(WebMessage,(PGM_P)pumpStateToText());
  strcat_P(WebMessage,(PGM_P)F(" ; PumpStatus:"));strcat_P(WebMessage,(PGM_P)pumpStatusToText());
  strcat_P(WebMessage,(PGM_P)F(" ; Interval:"));strcat(WebMessage,toText(MySettings.AeroInterval));
  strcat_P(WebMessage,(PGM_P)F(" ; Duration:"));strcat(WebMessage,toText(MySettings.AeroDuration));
  strcat_P(WebMessage,(PGM_P)F("\n\r Reservoir - "));  
  strcat_P(WebMessage,(PGM_P)F("PH:")); strcat(WebMessage,toText(PH));
  strcat_P(WebMessage,(PGM_P)F(" ; Reservoir:")); strcat(WebMessage,reservoirText);  
  //strcat_P(WebMessage,(PGM_P)F(" ; Moisture:")); strcat(WebMessage,toText(Moisture)); strcat_P(WebMessage,(PGM_P)F("% ")); strcat_P(WebMessage,(PGM_P)F("\n\r "));
  return WebMessage;
}

char * toText(int Number){
  static char ReturnChar[10] = ""; //9 digits + null terminator max
  itoa(Number, ReturnChar, 10);
  return ReturnChar;
}

char * toText(int Number1, int Number2,const char * Separator){  //function overloading: Same named function, different parameters
  static char ReturnChar[12] = ""; //5 digit + separator + 5 digit + null
  snprintf(ReturnChar,12,"%d%s%d",Number1,Separator,Number2);
  return ReturnChar;
}

char * toText(float Number){ 
  static char ReturnFloatChar[10] = "";  //6 digits + decimal sign + 2 decimals + null terminator
  dtostrf(Number, 4, 2, ReturnFloatChar); 
  return ReturnFloatChar;
}

char * toText(float Number1, float Number2,const char * Separator){
  static char ReturnChar[18] = ""; 
  static char Number2Char[8] = ""; //4 digits + decimal sign + 2 decimals + null terminator
  dtostrf(Number1, 4, 2, ReturnChar); 
  strcat(ReturnChar,Separator);
  dtostrf(Number2, 4, 2, Number2Char); 
  strcat(ReturnChar,Number2Char);
  return ReturnChar;
}

char * timetoText(int Hour, int Minute){
  static char ReturnChar[6] = ""; //2 digit + separator + 2 digit + null
  sprintf (ReturnChar, "%02u:%02u", Hour, Minute);
  return ReturnChar;
}
