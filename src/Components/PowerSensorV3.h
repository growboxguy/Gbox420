#pragma once

#define PZEM004_NO_SWSERIAL ///< Singals to the PZEM004Tv30 module to use HardwareSerial instead of SoftwareSerial
#include "PZEM004Tv30.h"    ///< 3rd party module for the PZEM004T V3.0 power sensor
#include "420Common.h"
#include "420Module.h"

class PowerSensorV3 : virtual public Common
{
public:
  PowerSensorV3(const __FlashStringHelper *Name, Module *Parent, HardwareSerial *SerialPort); ///< constructor
  void refresh_FiveSec();
  void report(bool FriendlyFormat = false);
  char *getPowerText(bool FriendlyFormat = false);
  char *getEnergyText(bool FriendlyFormat = false);
  char *getVoltageText(bool FriendlyFormat = false);
  char *getCurrentText(bool FriendlyFormat = false);
  char *getFrequencyText(bool FriendlyFormat = false);
  char *getPowerFactorText();

private:
  PZEM004Tv30 *Sensor; ///< for PZEM004T V3.0 model
  float Power;         ///< Power sensor - W
  float Energy;        ///< Power sensor - Wh Total consumption
  float Voltage;       ///< Power sensor - V
  float Current;       ///< Power sensor - A
  float Frequency;
  float PowerFactor;

protected:
  Module *Parent;
};