#pragma once

///< RollingAverage class: For smoothing sensor readings
///<
///< The RollingAverageDepth defines how many previous readings to keep. (!Memory intense!)

#include "Arduino.h"

/*! 
 *  \brief     Rolling average tracker for smoothing sensor readings. Supports float and int values only.
 *  \details   Keeps the last few readings and calculates an average. 
 *  \author    GrowBoxGuy
 *  \version   4.20
 *  \warning   Memory intense! Use it only where really necessary (Instable/noisy sensor readings)
 */
class RollingAverage
{
private:
  long Sum = 0;             ///< Sum of all stored historical values
  int *History;             ///< Array to store historical readings, RollingAverageDepth in Settings.h defines the queue depth
  uint8_t Oldest = 0;       ///< Points to the oldest reading
  bool ResetAverage = true; ///< Next reading will overwrite the previous readings (Resets the average to the current reading)

public:
  RollingAverage();                              ///< Constructor,  RollingAverageDepth in Settings.h defines the queue depth
  void resetAverage();                           ///< Flags to clear previously stored values at the next updateAverage() call
  int updateAverage(int LatestReading);          ///< Add an item to historical log and recalculate the average
  float updateAverage(float LatestReading);      ///< Add an item to the historical log and recalculate the average
  int getInt(bool ReturnAveragee = true);        ///< get the average as int, if false is passed return the latest reading
  float getFloat(bool ReturnAverage = true);     ///< get the average as float. if false is passed return the latest reading
  char *getIntText(bool ReturnAverage = true);   ///< get the average as int, formatted as text
  char *getFloatText(bool ReturnAverage = true); ///< get the average as float, formatted as text
};