 #include "Common.h" 

  static Settings Common::MySettings;
  static char Common::Message[512]; //initialize the temporary character buffer
  static char Common::CurrentTime[20]; 
   
  void Common::refresh(){  //Called when component should refresh its state 
    if(MySettings.DebugEnabled){logToSerials(F("Common object refreshing"),true);}
  }  

  static void addToLog(const __FlashStringHelper* Text){
    ;
  }

  static void addToLog(const char* Text){
    ;
  }

  static char * Common::getFormattedTime(){
    time_t Now = now(); // Get the current time and date from the TimeLib library
    snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d",year(Now), month(Now), day(Now),hour(Now), minute(Now), second(Now)); // YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
    return CurrentTime;
  }  
  
  static float Common::convertBetweenTempUnits(float Value){
  if(MySettings.MetricSystemEnabled) return round((Value-32) * 55.555555)/100.0;
  else return round(Value*180 + 00.0)/100.0;
  }
  
  static float Common::convertBetweenPressureUnits(float Value){
  if(MySettings.MetricSystemEnabled) return round(Value / 0.145038)/100.0;
  else return round(Value*1450.38)/100.0;
  }
  
  static char * Common::toText(int Number){
    static char ReturnChar[MaxTextLength] = "";
    itoa(Number, ReturnChar, 10);
    return ReturnChar;
  }

    static char * Common::toText(long Number){ 
    static char ReturnFloatChar[MaxTextLength] = ""; 
    sprintf (ReturnFloatChar, "%ld", Number);
    return ReturnFloatChar;
  }
  
  //char * Common::toText(uint_t Number){
  //  static char ReturnChar[MaxTextLength] = "";
  //  itoa(Number, ReturnChar, 10);
  //  return ReturnChar;
  //}
  
  static char * Common::toText(int Number1, int Number2,const char * Separator){  //function overloading: Same named function, different parameters
    static char ReturnChar[MaxTextLength] = ""; 
    snprintf(ReturnChar,32,"%d%s%d",Number1,Separator,Number2);
    return ReturnChar;
  }
  
  static char * Common::toText(float Number){ 
    static char ReturnFloatChar[MaxTextLength] = ""; 
    if(isnan(Number)) Number= -1.0;
    dtostrf(Number, 4, 2, ReturnFloatChar); //minimum 4 char total lengt (Including decimal and possible - sign), with 2 digits after the decimal point
    return ReturnFloatChar;
  }
  
  static char * Common::toPrecisionText(float Number){ 
    static char ReturnFloatChar[MaxTextLength] = "";
    if(isnan(Number)) Number= -1.0;
    dtostrf(Number, 8, 6, ReturnFloatChar); //minimum 8 char total lengt, with 6 decimals
    return ReturnFloatChar;
  }
  
  static char * Common::toText(float Number1, float Number2,const char * Separator){
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
  
  static char * Common::timeToText(byte Hour, byte Minute){
    static char ReturnChar[MaxTextLength] = ""; //2 digit + separator + 2 digit + null
    sprintf (ReturnChar, "%02u:%02u", Hour, Minute);
    return ReturnChar;
  }  

  static char * Common::tempToText(float Temp){
    static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same static variable
    dtostrf(Temp, 4, 2, ReturnChar); 
    if(MySettings.MetricSystemEnabled){      
      strcat_P(ReturnChar,(PGM_P)F("C"));
    }
    else{
      strcat_P(ReturnChar,(PGM_P)F("F"));
    }
    return ReturnChar;
  } 

  static char * Common::percentageToText(float Humidity){
    //char * ReturnChar = malloc(MaxTextLength * sizeof(char));  //allocate memory for every run - need to take care of freeing up the memory  after use
    static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same static variable
    dtostrf(Humidity, 4, 2, ReturnChar);      
    strcat_P(ReturnChar,(PGM_P)F("%"));  
    return ReturnChar; 
  }
  
  static const __FlashStringHelper * Common::onOffToText(bool Status){
     if(Status) return F("ON");
     else return F("OFF");   
  } 
  
  static const __FlashStringHelper * Common::yesNoToText(bool Status){
     if(Status) return F("YES");
     else return F("NO");
  }
  
  static const __FlashStringHelper * Common::statusToText(bool Status){
     if(Status) return F("OK");
     else return F("!!!");
  }
  
  static const __FlashStringHelper * Common::enabledToText(bool Status){
     if(Status) return F("ENABLED");
     else return F("DISABLED");
  }
  
  static void Common::setMetricSystemEnabled(bool MetricEnabled){  //DOES NOT BELONG HERE??
    if(MetricEnabled != MySettings.MetricSystemEnabled){  //if there was a change
      MySettings.MetricSystemEnabled = MetricEnabled;
      MySettings.InternalFanSwitchTemp = convertBetweenTempUnits(MySettings.InternalFanSwitchTemp);
      MySettings.TempAlertLow= convertBetweenTempUnits(MySettings.TempAlertLow);
      MySettings.TempAlertHigh= convertBetweenTempUnits(MySettings.TempAlertHigh);
      MySettings.PressureAlertLow=convertBetweenPressureUnits(MySettings.PressureAlertLow);
      MySettings.PressureAlertHigh=convertBetweenPressureUnits(MySettings.PressureAlertHigh);
    }    
  //  if(MySettings.MetricSystemEnabled) addToLog(F("Using Metric units"));
   // else addToLog(F("Using Imperial units"));  
  }  

//////////////////////////////////////////////////////////////////
//RollingAverage functions

 int RollingAverage::getAverageInt(){
  return (int)(Sum / RollingAverageQueueDepth);
 }
 
 float RollingAverage::getAverageFloat()
 {
  return (float)(Sum / RollingAverageQueueDepth) /100;
 }
 
 int RollingAverage::updateAverage(int LatestReading){
     Sum -= History[Oldest]; //remove the oldest reading from the total
     Sum += LatestReading; //Add the newest reading
     History[Oldest++] = LatestReading;  //replace the oldest reading, then move the pointer to the oldest entry 
     if(Oldest >= RollingAverageQueueDepth){ //reached the end of the queue
       Oldest = 0;
     }
     int Average = Sum / RollingAverageQueueDepth;
     if(Common::MySettings.DebugEnabled){  //THIS SECTION CAN BE REMOVED
       memset(&Common::Message[0], 0, sizeof(Common::Message));  //clear variable       
       strcat(Common::Message,Common::toText(Oldest));
       strcat_P(Common::Message,(PGM_P)F(":Reading:")); strcat(Common::Message,Common::toText(LatestReading)); 
       strcat_P(Common::Message,(PGM_P)F(",Sum:")); strcat(Common::Message,Common::toText(Sum));
       strcat_P(Common::Message,(PGM_P)F(",Average:")); strcat(Common::Message,Common::toText(Average));
       Common::logToSerials(Common::Message,true);
     }
      return Average;
   }

  float RollingAverage::updateAverage(float LatestReading){
    float temp = LatestReading * 100;
    int temp2 = updateAverage((int)temp);
    return (float)temp2/100;
  }
   
