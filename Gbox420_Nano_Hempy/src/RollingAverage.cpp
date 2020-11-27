#include "RollingAverage.h"
#include "Helpers.h"

RollingAverage::RollingAverage()
{
  History = new int[RollingAverageDepth];
}

void RollingAverage::resetAverage()
{
  ResetAverage = true;
}

int RollingAverage::getInt(bool ReturnAverage)
{
  if (ReturnAverage)
    return Sum / RollingAverageDepth;
  else
  {
    if (Oldest == 0)
      return History[RollingAverageDepth - 1];
    else
      return History[Oldest - 1];
  }
}

float RollingAverage::getFloat(bool ReturnAverage)
{
  if (ReturnAverage)
    return Sum / RollingAverageDepth / 100.0f;
  else
  {
    if (Oldest == 0)
      return History[RollingAverageDepth - 1] / 100.0f;
    else
      return History[Oldest - 1] / 100.0f;
  }
}

char *RollingAverage::getIntText(bool ReturnAverage)
{
  return toText(getInt(ReturnAverage));
}

char *RollingAverage::getFloatText(bool ReturnAverage)
{
  return toText(getFloat(ReturnAverage));
}

int RollingAverage::updateAverage(int LatestReading)
{
  if (ResetAverage)
  {
    Oldest = 0;
    for (int i = 0; i < RollingAverageDepth; i++)
    {
      History[i] = LatestReading;
    }
    Sum = (long)LatestReading * RollingAverageDepth;
    ResetAverage = false;
  }
  else
  {
    Sum -= History[Oldest];            ///remove the oldest reading from the total
    Sum += LatestReading;              ///Add the newest reading
    History[Oldest++] = LatestReading; ///replace the oldest reading, then move the pointer to the next oldest entry
    if (Oldest >= RollingAverageDepth)
    { ///reached the end of the queue
      Oldest = 0;
    }
  }
  int Average = Sum / RollingAverageDepth;
  return Average;
}

float RollingAverage::updateAverage(float LatestReading)
{
  int temp = updateAverage((int)(LatestReading * 100));
  return temp / 100.0f;
}