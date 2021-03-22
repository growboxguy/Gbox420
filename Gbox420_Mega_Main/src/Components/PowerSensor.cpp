#include "PowerSensor.h"

PowerSensor::PowerSensor(const __FlashStringHelper *Name, Module *Parent, HardwareSerial *SerialPort) : Common(Name)
{
  this->Parent = Parent;
  Sensor = new PZEM004T(SerialPort);
  PowerSensorIP = new IPAddress(192, 168, 1, 1);
  Sensor->setAddress(*PowerSensorIP); ///< start power meter
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("PowerSensor object created"), true, 3);
}

void PowerSensor::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  Voltage = Sensor->voltage(*PowerSensorIP);      ///< AC Voltage (V)
  Current = Sensor->current(*PowerSensorIP);      ///< Current (A)
  Power = Sensor->power(*PowerSensorIP);          ///< Actual power usage (W)
  Energy = Sensor->energy(*PowerSensorIP) / 1000; ///< total power consumption (kWh)
}

void PowerSensor::report(bool JSONReport)
{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"P\":\""));
    strcat(LongMessage, getPowerText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"E\":\""));
    strcat(LongMessage, getEnergyText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"V\":\""));
    strcat(LongMessage, getVoltageText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"C\":\""));
    strcat(LongMessage, getCurrentText(false));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
  }
  else //Print a report to the Serial console
  {
    Common::report();
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
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
}

char *PowerSensor::getPowerText(bool IncludeUnits)
{
  if (IncludeUnits)
  {
    static char ReturnChar[MaxWordLength] = ""; ///< each call will overwrite the same variable
    memset(&ReturnChar[0], 0, MaxWordLength);   ///< clear variable
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
    static char ReturnChar[MaxWordLength] = ""; ///< each call will overwrite the same variable
    memset(&ReturnChar[0], 0, MaxWordLength);   ///< clear variable
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
    static char ReturnChar[MaxWordLength] = ""; ///< each call will overwrite the same variable
    memset(&ReturnChar[0], 0, MaxWordLength);   ///< clear variable
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
    static char ReturnChar[MaxWordLength] = ""; ///< each call will overwrite the same variable
    memset(&ReturnChar[0], 0, MaxWordLength);   ///< clear variable
    strcat(ReturnChar, toText(Current));
    if (IncludeUnits)
      strcat_P(ReturnChar, (PGM_P)F("A"));
    return ReturnChar;
  }
  else
    return toText(Current);
}
