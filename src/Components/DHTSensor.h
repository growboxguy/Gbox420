#pragma once

#include "DHT.h" 
#include "420Common.h"
#include "420Module.h"

///Digital Humidity and Temperature sensor (DHT11 or DHT22)

class DHTSensor : virtual public Common
{
public:
  DHTSensor(const __FlashStringHelper *Name, Module *Parent, Settings::DHTSensorSettings *DefaultSettings);
  void refresh_Minute();
  void report();
  float getTemp(bool ReturnAverage = true);
  char *getTempText(bool IncludeUnits, bool ReturnAverage);
  float getHumidity(bool ReturnAverage = true);
  char *getHumidityText(bool IncludeUnits, bool ReturnAverage);
  RollingAverage *Temp;
  RollingAverage *Humidity;

private:
  
protected:
  Module *Parent;
  DHT *Sensor; ///Pointer declaration, points to null initially
};