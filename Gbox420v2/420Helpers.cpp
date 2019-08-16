#include "420Helpers.h"

//////////////////////////////////////////
//Logging



void logToSerials (const __FlashStringHelper* ToPrint,bool BreakLine) { 
  if(BreakLine){Serial.println(ToPrint);Serial3.println(ToPrint);}
  else{Serial.print(ToPrint);Serial3.print(ToPrint);}
}

//////////////////////////////////////////
//Settings

void saveSettings(bool LogThis){ //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void*)&MySettings, (void*)0, sizeof(MySettings)); //update_block only writes the bytes that changed
  if(LogThis) logToSerials(F("Settings saved to EEPROM"),true);
}

void loadSettings(){
  Settings EEPROMSettings; //temporary storage with "Settings" type
  eeprom_read_block((void*)&EEPROMSettings, (void*)0, sizeof(EEPROMSettings));  
  if(EEPROMSettings.Version != MySettings.Version){
    logToSerials(F("Change detected, updating EEPROM..."),false);
    saveSettings(false);  //overwrites stored settings with defaults from this sketch
  }
  else {
    logToSerials(F("Same structure version detected, applying restored settings..."),false);
    MySettings = EEPROMSettings; //overwrite sketch defaults with loaded settings
  }
  logToSerials(F("done"),true);
}

//////////////////////////////////////////
//Time

char * getFormattedTime(){
  time_t Now = now(); // Get the current time and date from the TimeLib library
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d",year(Now), month(Now), day(Now),hour(Now), minute(Now), second(Now)); // YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
  return CurrentTime;
}  

bool SyncInProgress=false;
time_t getNtpTime(){
  time_t NTPResponse = 0;
  if(!SyncInProgress){ //blocking calling the sync again in an interrupt
    SyncInProgress = true;
    uint32_t LastRefresh = millis();  
    logToSerials(F("Waiting for NTP time (30sec timeout)..."),false);  
    while(NTPResponse == 0 && millis() - LastRefresh < 30000){
     NTPResponse = ESPCmd.GetTime();
     delay(500);     
     wdt_reset(); //reset watchdog timeout
    }
    SyncInProgress = false;
    if(NTPResponse == 0) {
      logToSerials(F("NTP time sync failed"),true);
      //sendEmailAlert(F("NTP%20time%20sync%20failed")); 
    }
    else logToSerials(F("NTP time synchronized"),true);
    }
  return NTPResponse;
}

//////////////////////////////////////////
//Conversions

float convertBetweenTempUnits(float Value){
if(MySettings.MetricSystemEnabled) return round((Value-32) * 55.555555)/100.0;
else return round(Value*180 + 00.0)/100.0;
}

float convertBetweenPressureUnits(float Value){
  if(MySettings.MetricSystemEnabled) return round(Value / 0.145038)/100.0;
  else return round(Value*1450.38)/100.0;
}

//////////////////////////////////////////
//Text formating

char * toText(int Number){
  char ReturnChar[MaxTextLength] = "";
  itoa(Number, ReturnChar, 10);
  return ReturnChar;
}

  char * toText(long Number){ 
  char ReturnFloatChar[MaxTextLength] = ""; 
  sprintf (ReturnFloatChar, "%ld", Number);
  return ReturnFloatChar;
}

char * toText(int Number1, int Number2,const char * Separator){  //function overloading: Same named function, different parameters
  char ReturnChar[MaxTextLength] = ""; 
  snprintf(ReturnChar,32,"%d%s%d",Number1,Separator,Number2);
  return ReturnChar;
}

char * toText(float Number){ 
  char ReturnFloatChar[MaxTextLength] = ""; 
  if(isnan(Number)) Number= -1.0;
  dtostrf(Number, 4, 2, ReturnFloatChar); //minimum 4 char total lengt (Including decimal and possible - sign), with 2 digits after the decimal point
  return ReturnFloatChar;
}

char * toPrecisionText(float Number){ 
  char ReturnFloatChar[MaxTextLength] = "";
  if(isnan(Number)) Number= -1.0;
  dtostrf(Number, 8, 6, ReturnFloatChar); //minimum 8 char total lengt, with 6 decimals
  return ReturnFloatChar;
}

char * toText(float Number1, float Number2,const char * Separator){
  char ReturnChar[MaxTextLength] = ""; 
  char Number2Char[MaxTextLength] = "";
  if(isnan(Number1)) Number1= -1.0;
  if(isnan(Number2)) Number2= -1.0;
  dtostrf(Number1, 4, 2, ReturnChar); 
  strcat(ReturnChar,Separator);
  dtostrf(Number2, 4, 2, Number2Char); 
  strcat(ReturnChar,Number2Char);
  return ReturnChar;
}

char * timeToText(byte Hour, byte Minute){
  char ReturnChar[MaxTextLength] = ""; //2 digit + separator + 2 digit + null
  sprintf (ReturnChar, "%02u:%02u", Hour, Minute);
  return ReturnChar;
}  

char * tempToText(float Temp){
  char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  dtostrf(Temp, 4, 2, ReturnChar); 
  if(MySettings.MetricSystemEnabled){      
    strcat_P(ReturnChar,(PGM_P)F("°C"));
  }
  else{
    strcat_P(ReturnChar,(PGM_P)F("°F"));
  }
  return ReturnChar;
} 

char * percentageToText(float Number){
  //char * ReturnChar = malloc(MaxTextLength * sizeof(char));  //allocate memory for every run - need to take care of freeing up the memory  after use
  char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  dtostrf(Number, 4, 2, ReturnChar);      
  strcat_P(ReturnChar,(PGM_P)F("%"));  
  return ReturnChar; 
}

char * percentageToText(int Number){
  //char * ReturnChar = malloc(MaxTextLength * sizeof(char));  //allocate memory for every run - need to take care of freeing up the memory  after use
  char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  itoa(Number, ReturnChar, 10);      
  strcat_P(ReturnChar,(PGM_P)F("%"));  
  return ReturnChar; 
}

const __FlashStringHelper * onOffToText(bool Status){
   if(Status) return F("ON");
   else return F("OFF");   
} 

const __FlashStringHelper * yesNoToText(bool Status){
   if(Status) return F("YES");
   else return F("NO");
}

const __FlashStringHelper * statusToText(bool Status){
   if(Status) return F("OK");
   else return F("!!!");
}

const __FlashStringHelper * enabledToText(bool Status){
   if(Status) return F("ENABLED");
   else return F("DISABLED");
}

//////////////////////////////////////////
//Debug

char * getFreeMemory(){
  char ReturnChar[MaxTextLength] = "";
  itoa(freeMemory(), ReturnChar, 10);
  logToSerials(F("Free memory(bytes): "),false); logToSerials(&ReturnChar,true);
}
