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
  Serial.print("Reporting to Google Sheets: "); Serial.println(WebMessage);   
  RestAPI.get(WebMessage);
}

void logToSerial(){  
  Serial.print(CurrentTime);
  Serial.print(F("\tTempF: ")); Serial.print(BoxTempC); Serial.print(F("C"));
  Serial.print(F("\tTempF: ")); Serial.print(BoxTempF); Serial.print(F("F"));
  Serial.print(F("\tHumidity: ")); Serial.print(Humidity); Serial.print( F("%"));
  Serial.print(F("\tPower: ")); Serial.print(Power); Serial.println(F("W")); 
  Serial.print(F("\tEnergy: ")); Serial.print(Energy); Serial.print(F("Wh"));   
  Serial.print(F("\tVoltage: ")); Serial.print(Voltage); Serial.print(F("V"));
  Serial.print(F("\tCurrent: ")); Serial.print(Current); Serial.print(F("A"));
  Serial.print(F("\tPH: ")); Serial.print(PH);
  Serial.print(F("\tMoisture: ")); Serial.print(Moisture); Serial.println(F("% "));
  if(MySettings.isLightOn) Serial.print(F("\tLight is on"));else Serial.print(F("\tLight is off"));  
  if(isBright) Serial.print(F("\tIt is bright"));else Serial.print(F("\tIt is dark"));  
  Serial.print(F("\tBrightness: ")); Serial.print(MySettings.LightBrightness);
  Serial.print(F("\tLightReading: ")); Serial.print(LightReading); Serial.print(F(" - ")); Serial.print(LightReadingPercent); Serial.println(F("%"));
  Serial.print(F("\tReservoir: (")); Serial.print(reservoirToPercent());  Serial.print(F(")\t"));  Serial.print(reservoirToText(true));
  Serial.print(F("\tInternal fan: "));Serial.print(fanSpeedToText(true));
  Serial.print(F("\tExhaust fan: "));Serial.println(fanSpeedToText(false));
  Serial.print(F("\tPressure: "));Serial.print(AeroPressure);Serial.print("bar/");Serial.print(AeroPressurePSI);Serial.print("psi");
  Serial.print(F("\tLow: "));Serial.print(MySettings.AeroPressureLow);Serial.print(F("\tHigh: "));Serial.println(MySettings.AeroPressureHigh);
  Serial.print(F("\tAeroInterval: "));Serial.print(MySettings.AeroInterval);Serial.print(F("\tAeroDuration: "));Serial.print(MySettings.AeroDuration);Serial.print(F("\tAeroOffset: "));Serial.println(MySettings.AeroOffset);
}


void addToLog(const char *message){
  Serial.println(message);
  for(int i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldes
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],LogLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy(Logs[0],message,LogLength);  //instert new log to [0]
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
