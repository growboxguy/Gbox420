#pragma once

#include "PZEM004T.h" 
#include "420Common.h"
#include "420Module.h"

///PZEM004T power sensor

class PowerSensor : virtual public Common
{
public:
  PowerSensor(const __FlashStringHelper *Name, Module *Parent, HardwareSerial *SerialPort); ///constructor
  void refresh_FiveSec();
  void report();
  char *getPowerText(bool IncludeUnits);
  char *getEnergyText(bool IncludeUnits);
  char *getVoltageText(bool IncludeUnits);
  char *getCurrentText(bool IncludeUnits);

private:
  PZEM004T *Sensor;         ///for PZEM004T model
  IPAddress *PowerSensorIP; /// Power Sensor address (fake,just needs something set)
  float Power;              ///Power sensor - W
  float Energy;             ///Power sensor - Wh Total consumption
  float Voltage;            ///Power sensor - V
  float Current;            ///Power sensor - A

protected:
  Module* Parent;
};