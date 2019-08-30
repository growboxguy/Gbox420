char WebMessage[512];   //buffer for GoogleSheets report

void reportToGoogleSheets(){
  addToLog("Reporting to Google Sheets");
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat(WebMessage,"/pushingbox?devid=v3F6589573D444A2");
  strcat(WebMessage,"&BoxTemp=");  strcat(WebMessage,floatToChar(BoxTempC));
  strcat(WebMessage,"&Humidity=");  strcat(WebMessage,floatToChar(Humidity));
  strcat(WebMessage,"&Power=");  strcat(WebMessage,floatToChar(Wattage)); 
  strcat(WebMessage,"&Energy=");  strcat(WebMessage,floatToChar(Energy));
  strcat(WebMessage,"&Voltage=");  strcat(WebMessage,floatToChar(Voltage));
  strcat(WebMessage,"&Current=");  strcat(WebMessage,floatToChar(Current));
  strcat(WebMessage,"&PH=");  strcat(WebMessage,floatToChar(PH));
  strcat(WebMessage,"&Moisture=");  strcat(WebMessage,floatToChar(Moisture));
  strcat(WebMessage,"&isLightOn=");  strcat(WebMessage,intToChar(isLightOn));
  strcat(WebMessage,"&Brightness=");  strcat(WebMessage,intToChar(Brightness));
  strcat(WebMessage,"&LightReading=");  strcat(WebMessage,intToChar(LightReading));
  strcat(WebMessage,"&isBright=");  strcat(WebMessage,intToChar(isBright));
  strcat(WebMessage,"&Reservoir=");  strcat(WebMessage,intToChar(ReservoirToPercent()));
  Serial.println(WebMessage);   
  rest.get(WebMessage);
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

void logToSerial(){  
  Serial.print(CurrentDate);Serial.print(" ");Serial.print(CurrentTime);
  Serial.print(F("\tTempF: ")); Serial.print(BoxTempC); Serial.print(F("C"));
  Serial.print(F("\tTempF: ")); Serial.print(BoxTempF); Serial.print(F("F"));
  Serial.print(F("\tHumidity: ")); Serial.print(Humidity); Serial.print( F("%"));
  Serial.print(F("\tWattage: ")); Serial.print(Wattage); Serial.println(F("W")); 
  Serial.print(F("\tEnergy: ")); Serial.print(Energy); Serial.print(F("Wh"));   
  Serial.print(F("\tVoltage: ")); Serial.print(Voltage); Serial.print(F("V"));
  Serial.print(F("\tCurrent: ")); Serial.print(Current); Serial.print(F("A"));
  Serial.print(F("\tPH: ")); Serial.print(PH);
  Serial.print(F("\tMoisture: ")); Serial.print(Moisture); Serial.println(F("% "));
  if(isLightOn) Serial.print(F("\tLight is on"));else Serial.print(F("\tLight is off"));  
  if(isBright) Serial.print(F("\tIt is bright"));else Serial.print(F("\tIt is dark"));  
  Serial.print(F("\tBrightness: ")); Serial.print(Brightness);
  Serial.print(F("\tLightReading: ")); Serial.print(LightReading); Serial.print(F(" - ")); Serial.print( LightReadingPercent); Serial.println(F("%"));
  Serial.print(F("\tReservoir: (")); Serial.print(ReservoirToPercent());  Serial.print(F(") "));  Serial.println(ReservoirToText());
}

void logToScreen(){
  Screen.fillScreen(ILI9341_BLACK);
  Screen.setCursor(0, 0);
  Screen.setTextColor(ILI9341_GREEN);Screen.setTextSize(3);  
  Screen.println("GrowBoxGuy"); 
  Screen.setTextColor(ILI9341_WHITE);Screen.setTextSize(2);  
  Screen.print(CurrentDate);Screen.print(F(" "));Screen.println(CurrentTime);
  Screen.setTextSize(1);Screen.println("");
  Screen.setTextColor(ILI9341_ORANGE);Screen.setTextSize(3);
  Screen.print(BoxTempC); Screen.print(F("C,Hum:"));Screen.print(Humidity); Screen.println( F("%"));
  Screen.setTextSize(1);Screen.println("");
  Screen.setTextColor(ILI9341_BLUE);Screen.setTextSize(2);
  Screen.print(Wattage); Screen.print(F("W, Total:"));Screen.print(Energy); Screen.println(F("kWh"));   
  Screen.setTextSize(1);Screen.println("");
  Screen.setTextColor(ILI9341_RED);Screen.setTextSize(2);
  Screen.print(F("PH:")); Screen.print(PH);
  Screen.print(F(",Moisture:")); Screen.print(Moisture); Screen.println(F("% "));
  Screen.setTextSize(1);Screen.println("");
  Screen.setTextColor(ILI9341_YELLOW);Screen.setTextSize(2);
  if(isLightOn) Screen.print(F("Light is on"));else Screen.print(F("Light is off"));  
  if(isBright) Screen.println(F(", it is bright"));else Screen.println(F(", it is dark"));
  Screen.print(F("Intensity:")); Screen.print(Brightness);Screen.print(F(" ("));Screen.print(LightReading); Screen.print(F(" - ")); Screen.print( LightReadingPercent); Screen.println(F("%)"));
  Screen.setTextSize(1);Screen.println("");
  Screen.setTextColor(ILI9341_MAGENTA);Screen.setTextSize(3);
  Screen.print(F("On:")); Screen.print(LightOnHour);Screen.print(F(":"));Screen.print(LightOnMinute);Screen.print(F(",Off:")); Screen.print(LightOffHour);Screen.print(F(":"));Screen.println(LightOffMinute);
  Screen.print(ReservoirToText());
}

char * intToChar(int Number){
static char ReturnChar[8] = ""; //7 digits + null terminator
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

char * ReservoirToText(){
    static char ReturnChar [32]= "";
    strcpy(ReturnChar,"\tReservoir level: E[");
    if(WaterAboveCritical) strcat(ReturnChar,"#"); else strcat(ReturnChar,"-");
    if(WaterAboveLow) strcat(ReturnChar,"#"); else strcat(ReturnChar,"-");
    if(WaterAboveMedium) strcat(ReturnChar,"#"); else strcat(ReturnChar,"-");
    if(WaterAboveFull) strcat(ReturnChar,"#"); else strcat(ReturnChar,"-");
    strcat(ReturnChar,"]F");
    return ReturnChar;
}

int ReservoirToPercent(){
  if(WaterAboveCritical && WaterAboveLow && WaterAboveMedium && WaterAboveFull) return 100;
  else if(WaterAboveCritical && WaterAboveLow && WaterAboveMedium && !WaterAboveFull) return 75;
  else if(WaterAboveCritical && WaterAboveLow && !WaterAboveMedium && !WaterAboveFull) return 50;
  else if(WaterAboveCritical && !WaterAboveLow && !WaterAboveMedium && !WaterAboveFull) return 10;
  else if(!WaterAboveCritical && !WaterAboveLow && !WaterAboveMedium && !WaterAboveFull) return 0;
  else return -1;
}

File ConfigFile;
void SaveToSD(){
  Serial.println(F("Saving config to SD card"));
   if (SD.exists("conf.txt")) { SD.remove("conf.txt"); }
  ConfigFile = SD.open("conf.txt", FILE_WRITE);
  if (ConfigFile) {
    ConfigFile.print("isLightOn=");ConfigFile.println(isLightOn); 
    ConfigFile.print("isTimerEnabled=");ConfigFile.println(isTimerEnabled); 
    ConfigFile.print("isPCPowerSupplyOn=");ConfigFile.println(isPCPowerSupplyOn); 
    ConfigFile.print("Brightness=");ConfigFile.println(Brightness); 
    ConfigFile.print("LightOnHour=");ConfigFile.println(LightOnHour); 
    ConfigFile.print("LightOnMinute=");ConfigFile.println(LightOnMinute); 
    ConfigFile.print("LightOffHour=");ConfigFile.println(LightOffHour); 
    ConfigFile.print("LightOffMinute=");ConfigFile.println(LightOffMinute); 
    ConfigFile.close();
  } else addToLog("Error writing SD card");
}

void ReadFromSD(){
  Serial.println(F("Loading config from SD card"));
  ConfigFile = SD.open("conf.txt");
  if (ConfigFile) {
  char buffer[40]; // May need to be a bit bigger if you have long names
  byte index = 0;
  while (ConfigFile.available()) {
        char c = ConfigFile.read();
         if(c == '\n') { // if line break 
             LoadConfigLine(buffer);
             index = 0;
         }
         else{
             buffer[index++] = c;             
         }
      buffer[index] = '\0'; // Keep buffer NULL terminated      
    }   
   ConfigFile.close();
  }else addToLog("Error reading SD card");  
}  

void LoadConfigLine(char *buff){
   char *name = strtok(buff, "=");
   if(name)
   {
    char *value = strtok(NULL, "=");
    if(value)
    {
      Serial.print("\t");Serial.print(name);Serial.print("=");Serial.println(value);
      if(strcmp(name, "isLightOn") == 0){isLightOn = atoi(value);} //bool is based on a uint8
      else if(strcmp(name, "isTimerEnabled") == 0){isTimerEnabled = atoi(value);} 
      else if(strcmp(name, "isPCPowerSupplyOn") == 0){isPCPowerSupplyOn = atoi(value);}
      else if(strcmp(name, "Brightness") == 0){Brightness = atoi(value);}
      else if(strcmp(name, "LightOnHour") == 0){LightOnHour = atoi(value);}  
      else if(strcmp(name, "LightOnMinute") == 0){LightOnMinute = atoi(value);}
      else if(strcmp(name, "LightOffHour") == 0){LightOffHour = atoi(value);}
      else if(strcmp(name, "LightOffMinute") == 0){LightOffMinute = atoi(value);}      
      //else if(strcmp(name, "Float") == 0){float val = atof(value)};
    }        
   }  
}

