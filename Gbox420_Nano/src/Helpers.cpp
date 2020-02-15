#include "Helpers.h"

//////////////////////////////////////////
//Debug

void getFreeMemory()
{
  static char ReturnChar[MaxTextLength] = "";
  itoa(freeMemory(), ReturnChar, 10);
  logToSerials(F("Free memory(bytes): "), false, 2);
  logToSerials(&ReturnChar, true, 0);
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
  static char ReturnChar[MaxTextLength] = "";
  strcpy_P(ReturnChar, (PGM_P)FlashText);
  return ReturnChar;
}

char *toText(int Number)
{
  static char ReturnChar[MaxTextLength] = "";
  itoa(Number, ReturnChar, 10);
  return ReturnChar;
}

char *toText(long Number)
{
  static char ReturnFloatChar[MaxTextLength] = "";
  sprintf(ReturnFloatChar, "%ld", Number);
  return ReturnFloatChar;
}

char *toText(float Number)
{
  static char ReturnFloatChar[MaxTextLength] = "";
  if (isnan(Number))
    Number = -1.0;
  dtostrf(Number, 4, 2, ReturnFloatChar); //minimum 4 char total length (Including decimal and possible - sign), with 2 digits after the decimal point
  return ReturnFloatChar;
}

char *toPrecisionText(float Number)
{
  static char ReturnFloatChar[MaxTextLength] = "";
  if (isnan(Number))
    Number = -1.0;
  dtostrf(Number, 8, 6, ReturnFloatChar); //minimum 8 char total length, with 6 decimals
  return ReturnFloatChar;
}

char *toText(int Number1, int Number2, const char *Separator)
{
  static char ReturnChar[MaxTextLength] = "";
  snprintf(ReturnChar, 32, "%d%s%d", Number1, Separator, Number2);
  return ReturnChar;
}

char *toText(float Number1, float Number2, const char *Separator)
{ //function overloading: Same named function, different parameter type
  static char ReturnChar[MaxTextLength] = "";
  char Number2Char[MaxTextLength] = "";
  if (isnan(Number1))
    Number1 = -1.0;
  if (isnan(Number2))
    Number2 = -1.0;
  dtostrf(Number1, 4, 2, ReturnChar);
  strcat(ReturnChar, Separator);
  dtostrf(Number2, 4, 2, Number2Char);
  strcat(ReturnChar, Number2Char);
  return ReturnChar;
}

char *timeToText(byte Hour, byte Minute)
{
  static char ReturnChar[MaxTextLength] = ""; //2 digit + separator + 2 digit + null
  sprintf(ReturnChar, "%02u:%02u", Hour, Minute);
  return ReturnChar;
}

char *tempToText(float Temp)
{
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  dtostrf(Temp, 4, 2, ReturnChar);
  if (*Metric)
  {
    strcat_P(ReturnChar, (PGM_P)F("°C"));
  }
  else
  {
    strcat_P(ReturnChar, (PGM_P)F("°F"));
  }
  return ReturnChar;
}

char *pressureToText(float Pressure)
{
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same ReturnChar variable
  dtostrf(Pressure, 4, 2, ReturnChar);
  if (*Metric)
  {
    strcat_P(ReturnChar, (PGM_P)F("bar"));
  }
  else
  {
    strcat_P(ReturnChar, (PGM_P)F("psi"));
  }
  return ReturnChar;
}

char *weightToText(float Weight)
{
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same ReturnChar variable
  dtostrf(Weight, 4, 2, ReturnChar);
  if (*Metric)
  {
    strcat_P(ReturnChar, (PGM_P)F("kg"));
  }
  else
  {
    strcat_P(ReturnChar, (PGM_P)F("lbs"));
  }
  return ReturnChar;
}

char *percentageToText(float Number)
{
  //static char * ReturnChar = malloc(MaxTextLength * sizeof(char));  //allocate memory for every run - need to take care of freeing up the memory  after use
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  dtostrf(Number, 4, 2, ReturnChar);
  strcat_P(ReturnChar, (PGM_P)F("%"));
  return ReturnChar;
}

char *percentageToText(int Number)
{
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  itoa(Number, ReturnChar, 10);
  strcat_P(ReturnChar, (PGM_P)F("%"));
  return ReturnChar;
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