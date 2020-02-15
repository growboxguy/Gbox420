#include "Helpers.h"

//////////////////////////////////////////
//Debug

void getFreeMemory()
{
  itoa(freeMemory(), ShortMessage, 10);
  logToSerials(F("Free memory(bytes): "), false, 2);
  logToSerials(&ShortMessage, true, 0);
}


//////////////////////////////////////////
//Conversions

float convertBetweenTempUnits(float Value)
{
  logToSerials(F("DEBUG: "), false, 0);
  logToSerials(Value, true, 0);
  logToSerials(F("DEBUG: "), false, 0);
  logToSerials(*Metric, true, 0);
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

//////////////////////////////////////////
//Text formating

char *toText(const __FlashStringHelper *FlashText)
{ //Not every external library supports __FlashStringHelper class. This converts flash stored text to ram stored char *
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

char *toText(float Number)
{
  if (isnan(Number))
    Number = -1.0;
  dtostrf(Number, 4, 2, ShortMessage); //minimum 4 char total length (Including decimal and possible - sign), with 2 digits after the decimal point
  return ShortMessage;
}

char *toPrecisionText(float Number)
{
  if (isnan(Number))
    Number = -1.0;
  dtostrf(Number, 8, 6, ShortMessage); //minimum 8 char total length, with 6 decimals
  return ShortMessage;
}

char *toText(int Number1, int Number2, const char *Separator)
{
  snprintf(ShortMessage, 32, "%d%s%d", Number1, Separator, Number2);
  return ShortMessage;
}

char *toText(float Number1, float Number2, const char *Separator)
{ //function overloading: Same named function, different parameter type
  char Number2Char[MaxTextLength] = "";
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

char *timeToText(byte Hour, byte Minute)
{
  sprintf(ShortMessage, "%02u:%02u", Hour, Minute);
  return ShortMessage;
}

char *tempToText(float Temp)
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

char *pressureToText(float Pressure)
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

char *weightToText(float Weight)
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

char *percentageToText(float Number)
{
  //static char * ReturnChar = malloc(MaxTextLength * sizeof(char));  //allocate memory for every run - need to take care of freeing up the memory  after use
   dtostrf(Number, 4, 2, ShortMessage);
  strcat_P(ShortMessage, (PGM_P)F("%"));
  return ShortMessage;
}

char *percentageToText(int Number)
{
  itoa(Number, ShortMessage, 10);
  strcat_P(ShortMessage, (PGM_P)F("%"));
  return ShortMessage;
}

char *onOffToText(bool Status)
{
  if (Status)
    return (char *)"ON";
  else
    return (char *)"OFF";
}

char *yesNoToText(bool Status)
{
  if (Status)
    return (char *)"YES";
  else
    return (char *)"NO";
}

char *enabledToText(bool Status)
{
  if (Status)
    return (char *)"ENABLED";
  else
    return (char *)"DISABLED";
}