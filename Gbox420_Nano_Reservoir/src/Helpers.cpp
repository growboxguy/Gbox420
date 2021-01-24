#include "Helpers.h"

///< Debug

void getFreeMemory()
{
  itoa(freeMemory(), ShortMessage, 10);
  logToSerials(F("Free memory(bytes): "), false, 2);
  logToSerials(&ShortMessage, true, 0);
}

///< Conversions

float convertBetweenTempUnits(float Value)
{
  if (*Metric)
    return round((Value - 32) * 55.555555) / 100.0;
  else
    return round(Value * 180 + 3200.0) / 100.0f;
}

float convertBetweenPressureUnits(float Value)
{
  if (*Metric)
    return round(Value / 0.145038) / 100.0;
  else
    return round(Value * 1450.38) / 100.0f;
}

///< Text formating

char *toText(const __FlashStringHelper *FlashText)
{ ///< Not every external library supports __FlashStringHelper class. This converts flash stored text to ram stored char *
  strcpy_P(ShortMessage, (PGM_P)FlashText);
  return ShortMessage;
}

char *toText(int Number)
{
  itoa(Number, ShortMessage, 10);
  return ShortMessage;
}

char *toText(long Number)
{
  sprintf(ShortMessage, "%ld", Number);
  return ShortMessage;
}

char *toText(unsigned int Number)
{
  itoa(Number, ShortMessage, 10);
  return ShortMessage;
}

char *toText(float Number)
{
  if (isnan(Number))
    Number = -1.0;
  dtostrf(Number, 4, 2, ShortMessage); ///< minimum 4 char total length (Including decimal and possible - sign), with 2 digits after the decimal point
  return ShortMessage;
}

char *toText_floatDecimals(float Number)
{
  if (isnan(Number))
    Number = -1.0;
  dtostrf(Number, 8, 6, ShortMessage); ///< minimum 8 char total length, with 6 decimals
  return ShortMessage;
}

char *toText(int Number1, int Number2, const char *Separator)
{
  snprintf(ShortMessage, 32, "%d%s%d", Number1, Separator, Number2);
  return ShortMessage;
}

char *toText(float Number1, float Number2, const char *Separator)
{ ///< function overloading: Same named function, different parameter type
  char Number2Char[MaxWordLength] = "";
  if (isnan(Number1))
    Number1 = -1.0;
  if (isnan(Number2))
    Number2 = -1.0;
  dtostrf(Number1, 4, 2, ShortMessage);
  strcat(ShortMessage, Separator);
  dtostrf(Number2, 4, 2, Number2Char);
  strcat(ShortMessage, Number2Char);
  return ShortMessage;
}

char *toText_time(uint8_t Hour, uint8_t Minute)
{
  sprintf(ShortMessage, "%02u:%02u", Hour, Minute);
  return ShortMessage;
}

char *toText_temp(float Temp)
{
  dtostrf(Temp, 4, 2, ShortMessage);
  if (*Metric)
  {
    strcat_P(ShortMessage, (PGM_P)F("°C"));
  }
  else
  {
    strcat_P(ShortMessage, (PGM_P)F("°F"));
  }
  return ShortMessage;
}

char *toText_pressure(float Pressure)
{
  dtostrf(Pressure, 4, 2, ShortMessage);
  if (*Metric)
  {
    strcat_P(ShortMessage, (PGM_P)F("bar"));
  }
  else
  {
    strcat_P(ShortMessage, (PGM_P)F("psi"));
  }
  return ShortMessage;
}

char *toText_weight(float Weight)
{
  dtostrf(Weight, 4, 2, ShortMessage);
  if (*Metric)
  {
    strcat_P(ShortMessage, (PGM_P)F("kg"));
  }
  else
  {
    strcat_P(ShortMessage, (PGM_P)F("lbs"));
  }
  return ShortMessage;
}

char *toText_percentage(float Number)
{
  ///< static char * ReturnChar = malloc(MaxWordLength * sizeof(char));  ///< allocate memory for every run - need to take care of freeing up the memory  after use
  dtostrf(Number, 4, 2, ShortMessage);
  strcat_P(ShortMessage, (PGM_P)F("%"));
  return ShortMessage;
}

char *toText_percentage(int Number)
{
  itoa(Number, ShortMessage, 10);
  strcat_P(ShortMessage, (PGM_P)F("%"));
  return ShortMessage;
}

char *toText_minute(int Minute)
{
  itoa(Minute, ShortMessage, 10);
  strcat_P(ShortMessage, (PGM_P)F("min"));
  return ShortMessage;
}

char *toText_second(int Second)
{
  itoa(Second, ShortMessage, 10);
  strcat_P(ShortMessage, (PGM_P)F("sec"));
  return ShortMessage;
}

char *toText_distance(float Distance)
{
  dtostrf(Distance, 4, 2, ShortMessage);
  if (*Metric)
  {
    strcat_P(ShortMessage, (PGM_P)F("cm"));
  }
  else
  {
    strcat_P(ShortMessage, (PGM_P)F("inch"));
  }
  return ShortMessage;
}

char *toText_TDS(float TDS)
{
  dtostrf(TDS, 4, 2, ShortMessage);
  strcat_P(ShortMessage, (PGM_P)F("ppm"));
  return ShortMessage;
}

char *toText_yesNo(bool Status)
{
  if (Status)
    return toText(F("YES"));
  else
    return toText(F("NO"));
}

char *toText_enabledDisabled(bool Status)
{
  if (Status)
    return toText(F("ENABLED"));
  else
    return toText(F("DISABLED"));
}

char *toText_onOff(bool Status)
{
  if (Status)
    return toText(F("ON"));
  else
    return toText(F("OFF"));
}

char *toText_onOffDisabled(bool Enabled, bool OnStatus)
{
  if (!Enabled)
    return toText(F("DISABLED"));
  else
  {
    if (OnStatus)
      return toText(F("ON"));
    else
      return toText(F("OFF"));
  }
}

char *toText_onlineStatus(bool Status)
{
  if (Status)
    return toText(F("ONLINE"));
  else
    return toText(F("OFFLINE"));
}

char *toText_waterPumpState(WaterPumpStates State)
{
  switch (State)
  {
  case WaterPumpStates::DISABLED:
    return toText(F("DISABLED"));
    break;
  case WaterPumpStates::IDLE:
    return toText(F("IDLE"));
    break;
  case WaterPumpStates::RUNNING:
    return toText(F("RUNNING"));
    break;
  default:
    return toText(F("UNKNOWN"));
    break;
  }
}

char *toText_pressurePumpState(PressurePumpStates State)
{
  switch (State)
  {
  case PressurePumpStates::DISABLED:
    return toText(F("DISABLED"));
    break;
  case PressurePumpStates::IDLE:
    return toText(F("IDLE"));
    break;
  case PressurePumpStates::PRIMING:
    return toText(F("PRIMING"));
    break;
  case PressurePumpStates::RUNNING:
    return toText(F("RUNNING"));
    break;
  case PressurePumpStates::BLOWOFF:
    return toText(F("BLOWOFF"));
    break;
  case PressurePumpStates::MIXING:
    return toText(F("MIXING"));
    break;
  case PressurePumpStates::CLOSINGBYPASS:
    return toText(F("BYPASS"));
    break;
  case PressurePumpStates::BYPASSOPEN:
    return toText(F("BYPASSOPEN"));
    break;    
  default:
    return toText(F("UNKNOWN"));
    break;
  }
}