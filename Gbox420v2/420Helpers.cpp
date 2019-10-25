#include "420Helpers.h"
#include "GrowBox.h"

//////////////////////////////////////////
//Logging

void logToSerials (const __FlashStringHelper* ToPrint,bool BreakLine,byte Indent) {
  while(Indent>0){
     ArduinoSerial.print(F(" "));
     ESPSerial.print(F(" "));
     Indent--;
  }
  if(BreakLine){ArduinoSerial.println(ToPrint);ESPSerial.println(ToPrint);}
  else{ArduinoSerial.print(ToPrint);ESPSerial.print(ToPrint);}
}

//////////////////////////////////////////
//Time

char * getFormattedTime(){
  time_t Now = now(); // Get the current time and date from the TimeLib library
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d",year(Now), month(Now), day(Now),hour(Now), minute(Now), second(Now)); // YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
  return CurrentTime;
}  

static bool SyncInProgress=false;
time_t getNtpTime(){
  time_t NTPResponse = 0;
  if(!SyncInProgress){ //blocking calling the sync again in an interrupt
    SyncInProgress = true;
    uint32_t LastRefresh = millis();  
    logToSerials(F("Waiting for NTP time (15sec timeout)..."),false,0);  
    while(NTPResponse == 0 && millis() - LastRefresh < 15000){
     NTPResponse = ESPCmd.GetTime();
     delay(500);
     logToSerials(F("."),false,0);
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
 logToSerials(F("DEBUG: "),false,0); logToSerials(Value,true,0);
 logToSerials(F("DEBUG: "),false,0); logToSerials(GBox -> BoxSettings -> MetricSystemEnabled,true,0);
if(GBox -> BoxSettings -> MetricSystemEnabled) return round((Value-32) * 55.555555)/100.0;
else return round(Value*180 + 3200.0)/100.0f;
}

float convertBetweenPressureUnits(float Value){
  if(GBox -> BoxSettings -> MetricSystemEnabled) return round(Value / 0.145038)/100.0;
  else return round(Value*1450.38)/100.0f;
}

//////////////////////////////////////////
//Text formating

char * toText(const __FlashStringHelper * FlashText){   //Not every external library supports __FlashStringHelper class. This converts flash stored text to ram stored char *
  static char ReturnChar[MaxTextLength] = "";
  strcpy_P(ReturnChar, (PGM_P)FlashText);
  return ReturnChar;
}

char * toText(int Number){
  static char ReturnChar[MaxTextLength] = "";
  itoa(Number, ReturnChar, 10);
  return ReturnChar;
}

char * toText(long Number){ 
  static char ReturnFloatChar[MaxTextLength] = ""; 
  sprintf (ReturnFloatChar, "%ld", Number);
  return ReturnFloatChar;
}

char * toText(float Number){ 
  static char ReturnFloatChar[MaxTextLength] = ""; 
  if(isnan(Number)) Number= -1.0;
  dtostrf(Number, 4, 2, ReturnFloatChar); //minimum 4 char total length (Including decimal and possible - sign), with 2 digits after the decimal point
  return ReturnFloatChar;
}

char * toPrecisionText(float Number){ 
  static char ReturnFloatChar[MaxTextLength] = "";
  if(isnan(Number)) Number= -1.0;
  dtostrf(Number, 8, 6, ReturnFloatChar); //minimum 8 char total length, with 6 decimals
  return ReturnFloatChar;
}

char * toText(int Number1, int Number2,const char * Separator){ 
  static char ReturnChar[MaxTextLength] = ""; 
  snprintf(ReturnChar,32,"%d%s%d",Number1,Separator,Number2);
  return ReturnChar;
}

char * toText(float Number1, float Number2,const char * Separator){  //function overloading: Same named function, different parameter type
  static char ReturnChar[MaxTextLength] = ""; 
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
  static char ReturnChar[MaxTextLength] = ""; //2 digit + separator + 2 digit + null
  sprintf (ReturnChar, "%02u:%02u", Hour, Minute);
  return ReturnChar;
}  

char * tempToText(float Temp){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  dtostrf(Temp, 4, 2, ReturnChar); 
  if(GBox -> BoxSettings -> MetricSystemEnabled){      
    strcat_P(ReturnChar,(PGM_P)F("°C"));
  }
  else{
    strcat_P(ReturnChar,(PGM_P)F("°F"));
  }
  return ReturnChar;
} 

char * pressureToText(float Pressure){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same ReturnChar variable
  dtostrf(Pressure, 4, 2, ReturnChar); 
  if(GBox -> BoxSettings -> MetricSystemEnabled){      
    strcat_P(ReturnChar,(PGM_P)F("bar"));
  }
  else{
    strcat_P(ReturnChar,(PGM_P)F("psi"));
  }
  return ReturnChar;
} 

char * percentageToText(float Number){
  //static char * ReturnChar = malloc(MaxTextLength * sizeof(char));  //allocate memory for every run - need to take care of freeing up the memory  after use
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  dtostrf(Number, 4, 2, ReturnChar);      
  strcat_P(ReturnChar,(PGM_P)F("%"));  
  return ReturnChar; 
}

char * percentageToText(int Number){
  //char * ReturnChar = malloc(MaxTextLength * sizeof(char));  //allocate memory for every run - need to take care of freeing up the memory  after use
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  itoa(Number, ReturnChar, 10);      
  strcat_P(ReturnChar,(PGM_P)F("%"));  
  return ReturnChar; 
}

char * onOffToText(bool Status){
   if(Status) return (char *)"ON";
   else return (char *)"OFF";   
} 

char * yesNoToText(bool Status){
   if(Status) return (char *)"YES";
   else return (char *)"NO";
}

char * statusToText(bool Status){
   if(Status) return (char *)"OK";
   else return (char *)"!!!";
}

char * enabledToText(bool Status){
   if(Status) return (char *)"ENABLED";
   else return (char *)"DISABLED";
}

//////////////////////////////////////////
//Debug

void getFreeMemory(){
  static char ReturnChar[MaxTextLength] = "";
  itoa(freeMemory(), ReturnChar, 10);
  logToSerials(F("Free memory(bytes): "),false,2); logToSerials(&ReturnChar,true,0);
}

//////////////////////////////////////////////////////////////////
//RollingAverage functions

int RollingAverage::getAverageInt(){
  return Sum / RollingAverageQueueDepth;
}

float RollingAverage::getAverageFloat(){
  return Sum / RollingAverageQueueDepth /100.0f;
}

char * RollingAverage::getAverageIntText(){
  return toText(Sum / RollingAverageQueueDepth);
}

char * RollingAverage::getAverageFloatText(){
  return toText(Sum / RollingAverageQueueDepth /100.0f);
}

int RollingAverage::updateAverage(int LatestReading){
  if(ResetAverage){
    Oldest = 0;    
    for(int i=0;i<RollingAverageQueueDepth;i++){
      History[i] = LatestReading;
    }
    Sum = (long) LatestReading * RollingAverageQueueDepth;
    ResetAverage = false;
  }
  else
  {
   Sum -= History[Oldest]; //remove the oldest reading from the total
   Sum += LatestReading; //Add the newest reading
   History[Oldest++] = LatestReading;  //replace the oldest reading, then move the pointer to the oldest entry 
   if(Oldest >= RollingAverageQueueDepth){ //reached the end of the queue
     Oldest = 0;
   }   
  } 
  int Average = Sum / RollingAverageQueueDepth;        
    if(GBox -> BoxSettings -> DebugEnabled){ 
     memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable       
     strcat(LongMessage,toText(Oldest));
     strcat_P(LongMessage,(PGM_P)F(":Reading:")); strcat(LongMessage,toText(LatestReading)); 
     strcat_P(LongMessage,(PGM_P)F(",Sum:")); strcat(LongMessage,toText(Sum));
     strcat_P(LongMessage,(PGM_P)F(",Average:")); strcat(LongMessage,toText(Average));
     logToSerials(&LongMessage,true);     
   }      
   return Average;
}

float RollingAverage::updateAverage(float LatestReading){
  int temp = updateAverage((int)(LatestReading * 100));
  return temp/100.0f;
}

//////////////////////////////////////////////////////////////////
//AlertHandler functions

bool AlertHandler::reportHealth(bool ResultOK){  //true: Component passed checks, false: Failed at least one test
  logToSerials(F("Health log received: "),false);
  logToSerials(ResultOK,true);
  if(ResultOK) //true: Component passed checks
  {
    if(LastCheckResult == false) { TriggerCount = 0;} //If previous reading failed, but this one passed: Reset the counter of repeating results
    else {if(!HealthOK) {TriggerCount++;}} //If component is currently in failed status, count passed results
    
    if(!HealthOK && TriggerCount >= GBox -> BoxSettings -> TriggerCountBeforeAlert){  //If the same result came back multiple times (More than defined by TriggerCountBeforeAlert from the 420Settings.h ) change the component health state
      //sendEmailAlert(F("AC%20input%20recovered"));      
      logToSerials(F("Health recovered"),true);
      HealthOK = true;
    }    
  }
  else{ //false: Failed at least one test
    if(LastCheckResult == true){TriggerCount = 0;}  //If previous reading passed, but this one failed: Reset the counter of repeating results 
    else {if(HealthOK) {TriggerCount++;}}

    if(HealthOK && TriggerCount >= GBox -> BoxSettings -> TriggerCountBeforeAlert){
      //sendEmailAlert(F("AC%20input%20lost"));      
      logToSerials(F("Health check failed"),true);
      HealthOK = false;
    }
  }
  LastCheckResult = ResultOK;
  return HealthOK;
}