#pragma once

#include "DHT.h" ///DHT11 or DHT22 Digital Humidity and Temperature sensor
#include "420Common.h"
#include "420Module.h"

class DHTSensor : virtual public Common
{
public:
  DHTSensor(const __FlashStringHelper *Name, Module *Parent, Settings::DHTSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void report();
  void readSensor(); ///Refresh Temp and Humidity readings
  float getTemp();
  char *getTempText(bool IncludeUnits = false);
  float getHumidity();
  char *getHumidityText(bool IncludeUnits = false);
  float Temp = 0.0;
  float Humidity = 0.0;

private:
protected:
  Module *Parent;
  DHT *Sensor; ///Pointer declaration, points to null initially
};