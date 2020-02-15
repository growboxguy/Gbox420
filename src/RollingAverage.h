#pragma once

///RollingAverage class: For smoothing sensor readings
///Keeps the last few readings and calculates an average
///The RollingAverageDepth defines how many previous readings to keep. (!Memory intense!)

#include "Arduino.h"

class RollingAverage
{
private:
  long Sum = 0;
  int *History; ///array to store historical readings, 10 readings default
  byte Oldest = 0;                             ///Points to the oldest reading
  bool ResetAverage = true;                    ///Next reading will overwrite the previous readings (Resets the average to the current reading)

public:
  RollingAverage(); ///Constructor , default queue size is 10
  void resetAverage();
  int updateAverage(int LatestReading);
  float updateAverage(float LatestReading);
  int getInt(bool ReturnAveragee = true); ///by default return the average, if false is passed return the latest reading
  float getFloat(bool ReturnAverage = true);
  char *getIntText(bool ReturnAverage = true);
  char *getFloatText(bool ReturnAverage = true);
};