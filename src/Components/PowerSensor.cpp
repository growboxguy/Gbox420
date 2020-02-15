#include "PowerSensor.h"

PowerSensor::PowerSensor(const __FlashStringHelper *Name, Module *Parent, HardwareSerial *SerialPort) : Common(Name)
{
  this->Parent = Parent;
  Sensor = new PZEM004T(SerialPort);
  PowerSensorIP = new IPAddress(192, 168, 1, 1);
  Sensor->setAddress(*PowerSensorIP);    ///start power meter
  Parent->addToReportQueue(this);          ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_FiveSec(this); ///Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  logToSerials(F("PowerSensor object created"), true, 1);
}

void PowerSensor::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  Voltage = Sensor->voltage(*PowerSensorIP);      ///AC Voltage (V)
  Current = Sensor->current(*PowerSensorIP);      ///Current (A)
  Power = Sensor->power(*PowerSensorIP);          ///Actual power usage (W)
  Energy = Sensor->energy(*PowerSensorIP) / 1000; ///total power consumption (kWh)
}

void PowerSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Power:"));
  strcat(LongMessage, getPowerText(true));
  strcat_P(LongMessage, (PGM_P)F(" ; Total:"));
  strcat(LongMessage, getEnergyText(true));
  strcat_P(LongMessage, (PGM_P)F(" ; Voltage:"));
  strcat(LongMessage, getVoltageText(true));
  strcat_P(LongMessage, (PGM_P)F(" ; Current:"));
  strcat(LongMessage, getCurrentText(true));
  logToSerials(&LongMessage, true, 1);
}

char *PowerSensor::getPowerText(bool IncludeUnits)
{
  if (IncludeUnits)
  {
    static char ReturnChar[MaxTextLength] = "";    ///each call will overwrite the same variable
    memset(&ReturnChar[0], 0, sizeof(ReturnChar)); ///clear variable
    strcat(ReturnChar, toText(Power));
    if (IncludeUnits)
      strcat_P(ReturnChar, (PGM_P)F("W"));
    return ReturnChar;
  }
  else
    return toText(Power);
}

char *PowerSensor::getEnergyText(bool IncludeUnits)
{
  if (IncludeUnits)
  {
    static char ReturnChar[MaxTextLength] = "";    ///each call will overwrite the same variable
    memset(&ReturnChar[0], 0, sizeof(ReturnChar)); ///clear variable
    strcat(ReturnChar, toText(Energy));
    if (IncludeUnits)
      strcat_P(ReturnChar, (PGM_P)F("kWh"));
    return ReturnChar;
  }
  else
    return toText(Energy);
}

char *PowerSensor::getVoltageText(bool IncludeUnits)
{
  if (IncludeUnits)
  {
    static char ReturnChar[MaxTextLength] = "";    ///each call will overwrite the same variable
    memset(&ReturnChar[0], 0, sizeof(ReturnChar)); ///clear variable
    strcat(ReturnChar, toText(Voltage));
    if (IncludeUnits)
      strcat_P(ReturnChar, (PGM_P)F("V"));
    return ReturnChar;
  }
  else
    return toText(Voltage);
}

char *PowerSensor::getCurrentText(bool IncludeUnits)
{
  if (IncludeUnits)
  {
    static char ReturnChar[MaxTextLength] = "";    ///each call will overwrite the same variable
    memset(&ReturnChar[0], 0, sizeof(ReturnChar)); ///clear variable
    strcat(ReturnChar, toText(Current));
    if (IncludeUnits)
      strcat_P(ReturnChar, (PGM_P)F("A"));
    return ReturnChar;
  }
  else
    return toText(Current);
}
