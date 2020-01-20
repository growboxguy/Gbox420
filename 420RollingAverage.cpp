#include "420RollingAverage.h"
#include "420Helpers.h"
#include "GrowBox.h"

void RollingAverage::resetAverage(){
  ResetAverage = true;
}

int RollingAverage::getInt(bool ReturnAverage){
  if (ReturnAverage) return Sum / RollingAverageQueueDepth;
  else {
    if(Oldest == 0) return History[RollingAverageQueueDepth - 1];
    else return History[Oldest - 1];
  }
}

float RollingAverage::getFloat(bool ReturnAverage){
  if (ReturnAverage) return Sum / RollingAverageQueueDepth /100.0f;
  else{
    if(Oldest == 0) return History[RollingAverageQueueDepth - 1] /100.0f;
    else return History[Oldest - 1] /100.0f;
  }
}

char * RollingAverage::getIntText(bool ReturnAverage){
  return toText(getInt(ReturnAverage));
}

char * RollingAverage::getFloatText(bool ReturnAverage){
  return toText(getFloat(ReturnAverage));
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
   History[Oldest++] = LatestReading;  //replace the oldest reading, then move the pointer to the next oldest entry 
   if(Oldest >= RollingAverageQueueDepth){ //reached the end of the queue
     Oldest = 0;
   }   
  } 
  int Average = Sum / RollingAverageQueueDepth;        
    if(GBox != NULL && GBox -> BoxSettings -> DebugEnabled){ 
     memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable       
     strcat(LongMessage,toText(Oldest));
     strcat_P(LongMessage,(PGM_P)F(":Reading:")); strcat(LongMessage,toText(LatestReading)); 
     strcat_P(LongMessage,(PGM_P)F(",Sum:")); strcat(LongMessage,toText(Sum));
     strcat_P(LongMessage,(PGM_P)F(",Average:")); strcat(LongMessage,toText(Average));
     logToSerials(&LongMessage,true,4);     
   }      
   return Average;
}

float RollingAverage::updateAverage(float LatestReading){
  int temp = updateAverage((int)(LatestReading * 100));
  return temp/100.0f;
}