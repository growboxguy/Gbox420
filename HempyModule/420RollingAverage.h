#pragma once

//RollingAverage class: For smoothing sensor readings
//Keeps the last few readings and calculates an average
//In Gbox420Settings.h the RollingAverageQueueDepth defines how many previous readings to keep. (!Memory intense!)

#include "Arduino.h"
#include "Gbox420Settings.h"

class RollingAverage
{
private:
  long Sum = 0;
  int History[RollingAverageQueueDepth] = {0}; //array to store historical readings, 10 readings default
  byte Oldest = 0;                             //Points to the oldest reading
  bool ResetAverage = true;                    //Next reading will overwrite the previous readings (Resets the average to the current reading)

public:
  void resetAverage();
  int updateAverage(int LatestReading);
  float updateAverage(float LatestReading);
  int getInt(bool ReturnAveragee = true); //by default return the average, if false is passed return the latest reading
  float getFloat(bool ReturnAverage = true);
  char *getIntText(bool ReturnAverage = true);
  char *getFloatText(bool ReturnAverage = true);
};