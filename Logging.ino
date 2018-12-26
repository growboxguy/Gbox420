void LogToSerials(const char * ToPrint,bool breakLine){
  if(breakLine){
    Serial.println(ToPrint);
    Serial3.println(ToPrint);
  }
  else{
    Serial.print(ToPrint);
    Serial3.print(ToPrint);
  }
}

void LogToSerials(const __FlashStringHelper * ToPrint,bool breakLine){
  if(breakLine){
    Serial.println(ToPrint);
    Serial3.println(ToPrint);
  }
  else{
    Serial.print(ToPrint);
    Serial3.print(ToPrint);
  }
}

void LogToSerials(const int & ToPrint,bool breakLine){
  if(breakLine){
    Serial.println(ToPrint);
    Serial3.println(ToPrint);
  }
  else{
    Serial.print(ToPrint);
    Serial3.print(ToPrint);
  }
}

void LogToSerials(const long unsigned int & ToPrint,bool breakLine){
  if(breakLine){
    Serial.println(ToPrint);
    Serial3.println(ToPrint);
  }
  else{
    Serial.print(ToPrint);
    Serial3.print(ToPrint);
  }
}

void LogToSerials(const float & ToPrint,bool breakLine){
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

void addToLog(const __FlashStringHelper *message){
  LogToSerials(message,true);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],LogLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy_P(Logs[0],(PGM_P)message,LogLength);  //instert new log to [0]
}

void ReportToGoogleSheets(bool LogMessage){
  if(LogMessage)addToLog(F("Reporting to Google Sheets"));
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat_P(WebMessage,(PGM_P)F("/pushingbox?devid=")); strcat(WebMessage,PushingBoxDeviceID);
  strcat_P(WebMessage,(PGM_P)F("&BoxDate="));  strcat(WebMessage,CurrentTime);
  strcat_P(WebMessage,(PGM_P)F("&BoxTempC="));  strcat(WebMessage,floatToChar(BoxTempC));
  strcat_P(WebMessage,(PGM_P)F("&BoxTempF="));  strcat(WebMessage,floatToChar(BoxTempF));
  strcat_P(WebMessage,(PGM_P)F("&Humidity="));  strcat(WebMessage,floatToChar(Humidity));
  strcat_P(WebMessage,(PGM_P)F("&Power="));  strcat(WebMessage,floatToChar(Power)); 
  strcat_P(WebMessage,(PGM_P)F("&Energy="));  strcat(WebMessage,floatToChar(Energy));
  strcat_P(WebMessage,(PGM_P)F("&Voltage="));  strcat(WebMessage,floatToChar(Voltage));
  strcat_P(WebMessage,(PGM_P)F("&Current="));  strcat(WebMessage,floatToChar(Current));
  strcat_P(WebMessage,(PGM_P)F("&PH="));  strcat(WebMessage,floatToChar(PH));
  strcat_P(WebMessage,(PGM_P)F("&Moisture="));  strcat(WebMessage,floatToChar(Moisture));
  strcat_P(WebMessage,(PGM_P)F("&isLightOn="));  strcat(WebMessage,intToChar(MySettings.isLightOn));
  strcat_P(WebMessage,(PGM_P)F("&Brightness="));  strcat(WebMessage,intToChar(MySettings.LightBrightness));
  strcat_P(WebMessage,(PGM_P)F("&LightReading="));  strcat(WebMessage,intToChar(LightReading));
  strcat_P(WebMessage,(PGM_P)F("&isBright="));  strcat(WebMessage,intToChar(isBright));
  strcat_P(WebMessage,(PGM_P)F("&Reservoir="));  strcat(WebMessage,intToChar(reservoirToPercent()));
  strcat_P(WebMessage,(PGM_P)F("&InternalFan=")); strcat_P(WebMessage,(PGM_P)internalFanSpeedToText()); //strcat_P is the same as strcat, just for __FlashStringHelper type (stored in flash)
  strcat_P(WebMessage,(PGM_P)F("&ExhaustFan=")); strcat_P(WebMessage,(PGM_P)exhaustFanSpeedToText());
  LogToSerials(F("Reporting to Google Sheets: "),false); LogToSerials(WebMessage,true);   
  RestAPI.get(WebMessage);
}

void logToSerial(){  
  LogToSerials(CurrentTime,false);
  LogToSerials(F("\tTempC: "),false); LogToSerials(BoxTempC,false); LogToSerials(F("C"),false);
  LogToSerials(F("\tTempF: "),false); LogToSerials(BoxTempF,false); LogToSerials(F("F"),false);
  LogToSerials(F("\tHumidity: "),false); LogToSerials(Humidity,false); LogToSerials( F("%"),false);
  LogToSerials(F("\tPower: "),false); LogToSerials(Power,false); LogToSerials(F("W"),true); 
  LogToSerials(F("\tEnergy: "),false); LogToSerials(Energy,false); LogToSerials(F("Wh"),false);   
  LogToSerials(F("\tVoltage: "),false); LogToSerials(Voltage,false); LogToSerials(F("V"),false);
  LogToSerials(F("\tCurrent: "),false); LogToSerials(Current,false); LogToSerials(F("A"),false);
  LogToSerials(F("\tPH: "),false); LogToSerials(PH,false);
  LogToSerials(F("\tMoisture: "),false); LogToSerials(Moisture,false); LogToSerials(F("% "),true);
  LogToSerials(F("\tLight: "),false); LogToSerials(lightStatusToText(),false); 
  LogToSerials(F("\tBright: "),false); LogToSerials(isBrightToText(),false); 
  LogToSerials(F("\tBrightness: "),false); LogToSerials(MySettings.LightBrightness,false);
  LogToSerials(F("\tLightReading: "),false); LogToSerials(LightReading,false); LogToSerials(F(" - "),false); LogToSerials(LightReadingPercent,false); LogToSerials(F("%"),true);
  LogToSerials(F("\tLightON: "),false); LogToSerials(timeToChar(MySettings.LightOnHour, MySettings.LightOnMinute),false);LogToSerials(F("\tLightOFF: "),false); LogToSerials(timeToChar(MySettings.LightOffHour, MySettings.LightOffMinute),true);
  LogToSerials(F("\tInternal fan: "),false);LogToSerials(internalFanSpeedToText(),false);  LogToSerials(F("\tExhaust fan: "),false);LogToSerials(exhaustFanSpeedToText(),false);
  LogToSerials(F("\tReservoir: ("),false); LogToSerials(reservoirToPercent(),false);  LogToSerials(F(")\t"),false);  LogToSerials(reservoirToText(true),true);
  LogToSerials(F("\tPressure: "),false);LogToSerials(AeroPressure,false);LogToSerials(F("bar/"),false);LogToSerials(AeroPressurePSI,false);LogToSerials(F("psi"),false);
  LogToSerials(F("\tLow: "),false);LogToSerials(MySettings.AeroPressureLow,false);LogToSerials(F("\tHigh: "),false);LogToSerials(MySettings.AeroPressureHigh,false);
  LogToSerials(F("\tPumpState: "),false);LogToSerials(pumpStateToText(),false);LogToSerials(F("\tPumpStatus: "),false);LogToSerials(pumpStatusToText(),true);
  LogToSerials(F("\tAeroInterval: "),false);LogToSerials(MySettings.AeroInterval,false);LogToSerials(F("\tAeroDuration: "),false);LogToSerials(MySettings.AeroDuration,false);LogToSerials(F("\tAeroOffset: "),false);LogToSerials(MySettings.AeroOffset,true);
}

char * intToChar(int Number){
  static char ReturnChar[8] = ""; //7 digits + null terminator max
  itoa(Number, ReturnChar, 10);
  return ReturnChar;
}

char * intsToChar(int Number1, int Number2,const char * Separator){
  static char ReturnChar[12] = ""; //5 digit + separator + 5 digit + null
  snprintf(ReturnChar,12,"%d%s%d",Number1,Separator,Number2);
  return ReturnChar;
}

char * timeToChar(int Hour, int Minute){
  static char ReturnChar[6] = ""; //2 digit + separator + 2 digit + null
  sprintf (ReturnChar, "%02u:%02u", Hour, Minute);
  return ReturnChar;
}

char * floatToChar(float Number){ 
  static char ReturnFloatChar[8] = "";  //4 digits + decimal sign + 2 decimals + null terminator
  dtostrf(Number, 4, 2, ReturnFloatChar); 
  return ReturnFloatChar;
}

char * floatsToChar(float Number1, float Number2,const char * Separator){
  static char ReturnChar[18] = ""; 
  static char Number2Char[8] = ""; //4 digits + decimal sign + 2 decimals + null terminator
  dtostrf(Number1, 4, 2, ReturnChar); 
  strcat(ReturnChar,Separator);
  dtostrf(Number2, 4, 2, Number2Char); 
  strcat(ReturnChar,Number2Char);
  return ReturnChar;
}

const __FlashStringHelper * internalFanSpeedToText(){
   if(!MySettings.isInternalFanOn) return F("OFF");
   else if (MySettings.isInternalFanHigh) return F("HIGH");
   else return F("LOW");
}

const __FlashStringHelper * exhaustFanSpeedToText(){
   if(!MySettings.isExhaustFanOn) return F("OFF");
   else if (MySettings.isExhaustFanHigh) return F("HIGH");
   else return F("LOW");
}

const __FlashStringHelper * powerSupplyToText(){
   if(MySettings.isPCPowerSupplyOn) return F("ON");
   else return F("OFF");
}

char * reservoirToText(bool includeHeader){
  static char ReturnChar [32]= "";
  if(includeHeader){strcpy_P(ReturnChar,(PGM_P)F("Reservoir:E["));}
  else {strcpy_P(ReturnChar,(PGM_P)F("E["));}
  if(isWaterAboveCritical) strcat_P(ReturnChar,(PGM_P)F("#")); else strcat_P(ReturnChar,(PGM_P)F("-"));
  if(isWaterAboveLow) strcat_P(ReturnChar,(PGM_P)F("#")); else strcat_P(ReturnChar,(PGM_P)F("-"));
  if(isWaterAboveMedium) strcat_P(ReturnChar,(PGM_P)F("#")); else strcat_P(ReturnChar,(PGM_P)F("-"));
  if(isWaterAboveFull) strcat_P(ReturnChar,(PGM_P)F("#")); else strcat_P(ReturnChar,(PGM_P)F("-"));
  strcat_P(ReturnChar,(PGM_P)F("]F"));
  return ReturnChar;
}

int reservoirToPercent(){
  if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && isWaterAboveFull) return 100;
  else if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && !isWaterAboveFull) return 75;
  else if(isWaterAboveCritical && isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) return 50;
  else if(isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) return 25;
  else if(!isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) return 0;
  else return -1;
}
