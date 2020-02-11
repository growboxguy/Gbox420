#pragma once

#include "DHT.h" //DHT11 or DHT22 Digital Humidity and Temperature sensor
#include "420Common.h"
#include "../Modules/420Module.h"

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
  DHT *Sensor; //Pointer declaration, points to null initially
};