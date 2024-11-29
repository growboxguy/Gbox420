#include "PowerSensorV3.h"

PowerSensorV3::PowerSensorV3(const __FlashStringHelper *Name, Module *Parent, HardwareSerial *SerialPort) : Common(Name), Parent(Parent)
{
  Sensor = new PZEM004Tv30(SerialPort);
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("PowerSensorV3 ready"), true, 3);
}

void PowerSensorV3::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  Voltage = Sensor->voltage();     ///< AC Voltage (V)
  Current = Sensor->current();     ///< Current (A)
  Power = Sensor->power();         ///< Actual power usage (W)
  Energy = Sensor->energy();       ///< Total power consumption (kWh)
  Frequency = Sensor->frequency(); ///< Frequency (hz)
  PowerFactor = Sensor->pf();      ///< Power factor
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void PowerSensorV3::report(bool FriendlyFormat)
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
  strcat_P(LongMessage, (PGM_P)F("\",\"F\":\""));
  strcat(LongMessage, getFrequencyText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"PF\":\""));
  strcat(LongMessage, getPowerFactorText());
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

char *PowerSensorV3::getPowerText(bool FriendlyFormat)
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

char *PowerSensorV3::getEnergyText(bool FriendlyFormat)
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

char *PowerSensorV3::getVoltageText(bool FriendlyFormat)
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

char *PowerSensorV3::getCurrentText(bool FriendlyFormat)
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

char *PowerSensorV3::getFrequencyText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    static char ReturnChar[MaxWordLength] = ""; ///< each call will overwrite the same variable
    memset(&ReturnChar[0], 0, MaxWordLength);   ///< clear variable
    strcat(ReturnChar, toText(Frequency));
    strcat_P(ReturnChar, (PGM_P)F("Hz"));
    return ReturnChar;
  }
  else
    return toText(Frequency);
}

char *PowerSensorV3::getPowerFactorText()
{
  return toText(PowerFactor);
}