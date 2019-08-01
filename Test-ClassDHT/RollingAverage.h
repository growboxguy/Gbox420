#ifndef RollingAverage_H
#define RollingAverage_H

#include "Common.h"

class RollingAverage
{  
  private:
    long Sum = 0;
    int History[RollingAverageQueueDepth] = {0}; //array to store historical readings, 10 readings default
    byte Oldest = 0; //Points to the oldest reading      
   
  public:  
    int updateAverage(int LatestReading);
    float updateAverage(float LatestReading);
    int getAverageInt();
    float getAverageFloat();
};

#endif
