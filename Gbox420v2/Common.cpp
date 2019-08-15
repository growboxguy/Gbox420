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
     /* if(MySettings.DebugEnabled){  
       memset(&Message[0], 0, sizeof(Message));  //clear variable       
       strcat(Message,toText(Oldest));
       strcat_P(Message,(PGM_P)F(":Reading:")); strcat(Message,toText(LatestReading)); 
       strcat_P(Message,(PGM_P)F(",Sum:")); strcat(Message,toText(Sum));
       strcat_P(Message,(PGM_P)F(",Average:")); strcat(Message,toText(Average));
       logToSerials(&Message,true);  
      }  */    
      return Average;
 }

  float RollingAverage::updateAverage(float LatestReading){
    int temp = updateAverage((int)(LatestReading * 100));
    return (float)temp/100;
  }
   
