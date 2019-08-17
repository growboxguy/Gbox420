 #include "Common.h" 

  static char Logs[LogDepth][MaxTextLength];  //two dimensional array for storing log histroy (array of char arrays)
   
  void Common::refresh(){  //Called when component should refresh its state 
    if(MySettings.DebugEnabled){logToSerials(F("Common object refreshing"),true);}
  }  

  static void Common::addToLog(const char *message){  //adds a log entry that is displayed on the web interface
    logToSerials(message,true);
    for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
       memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
       strncpy(Logs[i],Logs[i-1],MaxTextLength ) ; 
      }  
    memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
    strncpy(Logs[0],message,MaxTextLength);  //instert new log to [0]
  }
  
  static void Common::addToLog(const __FlashStringHelper *message){ //function overloading: same function name, different parameter type 
    logToSerials(message,true);
    for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
       memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
       strncpy(Logs[i],Logs[i-1],MaxTextLength ) ; 
      }  
    memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
    strncpy_P(Logs[0],(PGM_P)message,MaxTextLength);  //instert new log to [0]
  }

  char* Common::eventLogToJSON(bool Append){ //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  if(!Append)memset(&Message[0], 0, sizeof(Message));
  strcat_P(Message,(PGM_P)F("["));
  for(int i=LogDepth-1;i >=0 ; i--)
  {
   strcat_P(Message,(PGM_P)F("\""));
   strcat(Message,Logs[i]);
   strcat_P(Message,(PGM_P)F("\""));
   if(i > 0 ) strcat_P(Message,(PGM_P)F(","));
  }
  Message[strlen(Message)] = ']';
  return Message;
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
    if(MySettings.MetricSystemEnabled) addToLog(F("Using Metric units"));
    else addToLog(F("Using Imperial units"));  
  }  
