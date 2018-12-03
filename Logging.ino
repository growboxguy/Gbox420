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

void LogToSerials(const long unsigned int& ToPrint,bool breakLine){
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
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldes
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],LogLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy(Logs[0],message,LogLength);  //instert new log to [0]
}

void ReportToGoogleSheets(bool LogMessage){
  if(LogMessage)addToLog("Reporting to Google Sheets");
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat(WebMessage,"/pushingbox?devid="); strcat(WebMessage,PushingBoxDeviceID);
  strcat(WebMessage,"&BoxDate=");  strcat(WebMessage,CurrentTime);
  strcat(WebMessage,"&BoxTempC=");  strcat(WebMessage,floatToChar(BoxTempC));
  strcat(WebMessage,"&BoxTempF=");  strcat(WebMessage,floatToChar(BoxTempF));
  strcat(WebMessage,"&Humidity=");  strcat(WebMessage,floatToChar(Humidity));
  strcat(WebMessage,"&Power=");  strcat(WebMessage,floatToChar(Power)); 
  strcat(WebMessage,"&Energy=");  strcat(WebMessage,floatToChar(Energy));
  strcat(WebMessage,"&Voltage=");  strcat(WebMessage,floatToChar(Voltage));
  strcat(WebMessage,"&Current=");  strcat(WebMessage,floatToChar(Current));
  strcat(WebMessage,"&PH=");  strcat(WebMessage,floatToChar(PH));
  strcat(WebMessage,"&Moisture=");  strcat(WebMessage,floatToChar(Moisture));
  strcat(WebMessage,"&isLightOn=");  strcat(WebMessage,intToChar(MySettings.isLightOn));
  strcat(WebMessage,"&Brightness=");  strcat(WebMessage,intToChar(MySettings.LightBrightness));
  strcat(WebMessage,"&LightReading=");  strcat(WebMessage,intToChar(LightReading));
  strcat(WebMessage,"&isBright=");  strcat(WebMessage,intToChar(isBright));
  strcat(WebMessage,"&Reservoir=");  strcat(WebMessage,intToChar(reservoirToPercent()));
  strcat(WebMessage,"&InternalFan="); strcat(WebMessage,fanSpeedToText(true));
  strcat(WebMessage,"&ExhaustFan="); strcat(WebMessage,fanSpeedToText(false)); 
  LogToSerials("Reporting to Google Sheets: ",false); LogToSerials(WebMessage,true);   
  RestAPI.get(WebMessage);
}

void logToSerial(){  
  LogToSerials(CurrentTime,false);
  LogToSerials(F("\tTempF: "),false); LogToSerials(BoxTempC,false); LogToSerials(F("C"),false);
  LogToSerials(F("\tTempF: "),false); LogToSerials(BoxTempF,false); LogToSerials(F("F"),false);
  LogToSerials(F("\tHumidity: "),false); LogToSerials(Humidity,false); LogToSerials( F("%"),false);
  LogToSerials(F("\tPower: "),false); LogToSerials(Power,false); LogToSerials(F("W"),true); 
  LogToSerials(F("\tEnergy: "),false); LogToSerials(Energy,false); LogToSerials(F("Wh"),false);   
  LogToSerials(F("\tVoltage: "),false); LogToSerials(Voltage,false); LogToSerials(F("V"),false);
  LogToSerials(F("\tCurrent: "),false); LogToSerials(Current,false); LogToSerials(F("A"),false);
  LogToSerials(F("\tPH: "),false); LogToSerials(PH,false);
  LogToSerials(F("\tMoisture: "),false); LogToSerials(Moisture,false); LogToSerials(F("% "),true);
  if(MySettings.isLightOn) LogToSerials(F("\tLight is on"),false);else LogToSerials(F("\tLight is off"),false);  
  if(isBright) LogToSerials(F("\tIt is bright"),false);else LogToSerials(F("\tIt is dark"),false);  
  LogToSerials(F("\tBrightness: "),false); LogToSerials(MySettings.LightBrightness,false);
  LogToSerials(F("\tLightReading: "),false); LogToSerials(LightReading,false); LogToSerials(F(" - "),false); LogToSerials(LightReadingPercent,false); LogToSerials(F("%"),true);
  LogToSerials(F("\tInternal fan: "),false);LogToSerials(fanSpeedToText(true),false);  LogToSerials(F("\tExhaust fan: "),false);LogToSerials(fanSpeedToText(false),false);
  LogToSerials(F("\tReservoir: ("),false); LogToSerials(reservoirToPercent(),false);  LogToSerials(F(")\t"),false);  LogToSerials(reservoirToText(true),true);
  LogToSerials(F("\tPressure: "),false);LogToSerials(AeroPressure,false);LogToSerials("bar/",false);LogToSerials(AeroPressurePSI,false);LogToSerials("psi",false);
  LogToSerials(F("\tLow: "),false);LogToSerials(MySettings.AeroPressureLow,false);LogToSerials(F("\tHigh: "),false);LogToSerials(MySettings.AeroPressureHigh,true);
  LogToSerials(F("\tAeroInterval: "),false);LogToSerials(MySettings.AeroInterval,false);LogToSerials(F("\tAeroDuration: "),false);LogToSerials(MySettings.AeroDuration,false);LogToSerials(F("\tAeroOffset: "),false);LogToSerials(MySettings.AeroOffset,true);
}



char * intToChar(int Number){
  static char ReturnChar[8] = ""; //7 digits + null terminator max
  itoa(Number, ReturnChar, 10);
  return ReturnChar;
}

char * intsToChar(int Number1, int Number2,const char * Separator){
  static char ReturnChar[12] = ""; //4 digit + separator + 4 digit + null
  static char Number2Char[5] = ""; //4 digits + null
  itoa(Number1, ReturnChar, 10);
  strcat(ReturnChar,Separator);
  itoa(Number2, Number2Char, 10);
  strcat(ReturnChar,Number2Char);
  return ReturnChar;
}

char * floatToChar(float Number){  //4 digits + decimal sign + 2 decimals + null terminator
  static char ReturnFloatChar[8] = "";
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

char * fanSpeedToText(bool Internal){
  static char ReturnChar [5]= "";
  if(Internal){
   if(!MySettings.isInternalFanOn) strcpy(ReturnChar,"OFF"); else if (MySettings.isInternalFanHigh) strcpy(ReturnChar,"HIGH"); else strcpy(ReturnChar,"LOW");
  }
  else{
   if(!MySettings.isExhaustFanOn) strcpy(ReturnChar,"OFF"); else if (MySettings.isExhaustFanHigh) strcpy(ReturnChar,"HIGH"); else strcpy(ReturnChar,"LOW");
  }
  return ReturnChar;
}

char * reservoirToText(bool includeHeader){
  static char ReturnChar [32]= "";
  if(includeHeader){strcpy(ReturnChar,"Reservoir: E[");}
  else {strcpy(ReturnChar,"E[");}
  if(isWaterAboveCritical) strcat(ReturnChar,"#"); else strcat(ReturnChar,"-");
  if(isWaterAboveLow) strcat(ReturnChar,"#"); else strcat(ReturnChar,"-");
  if(isWaterAboveMedium) strcat(ReturnChar,"#"); else strcat(ReturnChar,"-");
  if(isWaterAboveFull) strcat(ReturnChar,"#"); else strcat(ReturnChar,"-");
  strcat(ReturnChar,"]F");
  return ReturnChar;
}

int reservoirToPercent(){
  if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && isWaterAboveFull) return 100;
  else if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && !isWaterAboveFull) return 75;
  else if(isWaterAboveCritical && isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) return 50;
  else if(isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) return 10;
  else if(!isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) return 0;
  else return -1;
}
