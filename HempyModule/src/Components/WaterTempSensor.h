#pragma once

#include "OneWire.h"           //DS18B20 waterproof temperature sensor
#include "DallasTemperature.h" //DS18B20 waterproof temperature sensor
#include "420Common.h"
#include "420Module.h"

class WaterTempSensor : virtual public Common
{
public:
  WaterTempSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WaterTempSensorSettings *DefaultSettings); //constructor
    void refresh_Minute();
  void report();
  float getTemp(bool ReturnAverage = true);
  char *getTempText(bool IncludeUnits, bool ReturnAverage);
  RollingAverage *Temp;

private:  
  OneWire *TempSensorWire;
  DallasTemperature *TempSensor;

protected:
  Module* Parent;
};