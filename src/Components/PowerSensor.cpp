#include "PowerSensor.h"

PowerSensor::PowerSensor(const __FlashStringHelper *Name, Module *Parent, HardwareSerial *SerialPort) : Common(Name)
{
  this->Parent = Parent;
  Sensor = new PZEM004T(SerialPort);
  PowerSensorIP = new IPAddress(192, 168, 1, 1);
  Sensor->setAddress(*PowerSensorIP); ///< start power meter
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("PowerSensor ready"), true, 3);
}

void PowerSensor::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  Voltage = Sensor->voltage(*PowerSensorIP);      ///< AC Voltage (V)
  Current = Sensor->current(*PowerSensorIP);      ///< Current (A)
  Power = Sensor->power(*PowerSensorIP);          ///< Actual power usage (W)
  Energy = Sensor->energy(*PowerSensorIP) / 1000; ///< total power consumption (kWh)
}

void PowerSensor::report(bool FriendlyFormat)
{
  //if (FriendlyFormat) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"P\":\""));
    strcat(LongMessage, getPowerText(FriendlyFormat));
    strcat_P(LongMessage, (PGM_P)F("\",\"E\":\""));
    strcat(LongMessage, getEnergyText(FriendlyFormat));
    strcat_P(LongMessage, (PGM_P)F("\",\"V\":\""));
    strcat(LongMessage, getVoltageText(FriendlyFormat));
    strcat_P(LongMessage, (PGM_P)F("\",\"C\":\""));
    strcat(LongMessage, getCurrentText(FriendlyFormat));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  /*
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
  */
}

char *PowerSensor::getPowerText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    static char ReturnChar[MaxWordLength] = ""; ///< each call will overwrite the same variable
    memset(&ReturnChar[0], 0, MaxWordLength);   ///< clear variable
    strcat(ReturnChar, toText(Power));
    if (FriendlyFormat)
      strcat_P(ReturnChar, (PGM_P)F("W"));
    return ReturnChar;
  }
  else
    return toText(Power);
}

char *PowerSensor::getEnergyText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    static char ReturnChar[MaxWordLength] = ""; ///< each call will overwrite the same variable
    memset(&ReturnChar[0], 0, MaxWordLength);   ///< clear variable
    strcat(ReturnChar, toText(Energy));
    if (FriendlyFormat)
      strcat_P(ReturnChar, (PGM_P)F("kWh"));
    return ReturnChar;
  }
  else
    return toText(Energy);
}

char *PowerSensor::getVoltageText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    static char ReturnChar[MaxWordLength] = ""; ///< each call will overwrite the same variable
    memset(&ReturnChar[0], 0, MaxWordLength);   ///< clear variable
    strcat(ReturnChar, toText(Voltage));
    if (FriendlyFormat)
      strcat_P(ReturnChar, (PGM_P)F("V"));
    return ReturnChar;
  }
  else
    return toText(Voltage);
}

char *PowerSensor::getCurrentText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    static char ReturnChar[MaxWordLength] = ""; ///< each call will overwrite the same variable
    memset(&ReturnChar[0], 0, MaxWordLength);   ///< clear variable
    strcat(ReturnChar, toText(Current));
    if (FriendlyFormat)
      strcat_P(ReturnChar, (PGM_P)F("A"));
    return ReturnChar;
  }
  else
    return toText(Current);
}
