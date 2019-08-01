#include "RollingAverage.h"

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
     if(Common::MySettings.DebugEnabled){
       memset(&Common::Message[0], 0, sizeof(Common::Message));  //clear variable       
       strcat(Common::Message,Common::toText(Oldest));
       strcat_P(Common::Message,(PGM_P)F(":Reading:")); strcat(Common::Message,Common::toText(LatestReading)); 
       strcat_P(Common::Message,(PGM_P)F(",Sum:")); strcat(Common::Message,Common::toText(Sum));
       strcat_P(Common::Message,(PGM_P)F(",Average:")); strcat(Common::Message,Common::toText(Average));
       Serial.println(Common::Message);
       //logToSerials(Common::Message,true);
     }
      return Average;
   }

  float RollingAverage::updateAverage(float LatestReading){
    float temp = LatestReading * 100;
    int temp2 = updateAverage((int)temp);
    return (float)temp2/100;
  }
