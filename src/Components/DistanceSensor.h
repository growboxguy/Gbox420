#pragma once

#include "420Common.h"
#include "420Module.h"

class DistanceSensor : virtual public Common
{
public:
  DistanceSensor(const __FlashStringHelper *Name, Module *Parent, Settings::DistanceSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void report(bool FriendlyFormat = false);
  void readSensor(); ///< Refresh Temp and Humidity readings
  float getDistance();
  char *getDistanceText(bool FriendlyFormat = false);
  float Distance = 0.0;

private:
  uint8_t *EchoPin;
  uint8_t *TriggerPin;

protected:
  Module *Parent;
};