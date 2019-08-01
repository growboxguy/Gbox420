//These are shared functions by all modules
//They help converting to text, getting the current time, converting between metric and imperial systems...etc 

class RollingAvarage
{
  private:
    int Sum = 0;
    int History[RollingAverageQueueDepth] = {0}; //array to store historical readings
    byte Oldest = 0; //Points to the oldest reading       

  public:
    RollingAvarage(){}
    
   int updateAverage(int LatestReading){
     Sum -= History[Oldest]; //remove the oldest reading from the total
     Sum += LatestReading; //Add the newest reading
     History[Oldest++] = LatestReading;  //replace the oldest reading, then move the pointer to the oldest entry 
     if(Oldest >= RollingAverageQueueDepth){ //reached the end of the queue
       Oldest = 0;
     }
     int Average = Sum / RollingAverageQueueDepth;
     if(MySettings.DebugEnabled){
       memset(&Message[0], 0, sizeof(Message));  //clear variable 
       strcat(Message,toText(Oldest));
       strcat_P(Message,(PGM_P)F(":Reading:")); strcat(Message,toText(LatestReading)); 
       strcat_P(Message,(PGM_P)F(",Sum:")); strcat(Message,toText(Sum));
       strcat_P(Message,(PGM_P)F(",Average:")); strcat(Message,toText(Average));
       logToSerials(Message,true);
     }
      return Average;
   }

  float updateAverage(float LatestReading){
    float temp = LatestReading * 100;
    int temp2 = updateAverage((int)temp);
    return (float)temp2/100;
  }
   
};

char * getFormattedTime(){
  time_t Now = now(); // Get the current time and date from the TimeLib library
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d",year(Now), month(Now), day(Now),hour(Now), minute(Now), second(Now)); // YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
  return CurrentTime;
}

void setMetricSystemEnabled(bool MetricEnabled){
  if(MetricEnabled != MySettings.MetricSystemEnabled){  //if there was a change
    MySettings.MetricSystemEnabled = MetricEnabled;
    MySettings.InternalFanSwitchTemp = convertBetweenTempUnits(MySettings.InternalFanSwitchTemp);
    MySettings.TempAlertLow= convertBetweenTempUnits(MySettings.TempAlertLow);
    MySettings.TempAlertHigh= convertBetweenTempUnits(MySettings.TempAlertHigh);
    MySettings.PressureAlertLow=convertBetweenPressureUnits(MySettings.PressureAlertLow);
    MySettings.PressureAlertHigh=convertBetweenPressureUnits(MySettings.PressureAlertHigh);
  }    
  if(MySettings.MetricSystemEnabled) addToLog(F("Using Metric units"));
  else addToLog(F("Using Imperial units"));  
}

float convertBetweenTempUnits(float Value){
if(MySettings.MetricSystemEnabled) return round((Value-32) * 55.555555)/100.0;
else return round(Value*180 + 3200.0)/100.0;
}

float convertBetweenPressureUnits(float Value){
if(MySettings.MetricSystemEnabled) return round(Value / 0.145038)/100.0;
else return round(Value*1450.38)/100.0;
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

const __FlashStringHelper * onOffToText(bool Status){
   if(Status) return F("ON");
   else return F("OFF");   
} 

const __FlashStringHelper * yesNoToText(){
   if(Bright) return F("YES");
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
