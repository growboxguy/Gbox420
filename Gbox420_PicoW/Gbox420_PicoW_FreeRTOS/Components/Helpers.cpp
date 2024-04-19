#include "Helpers.h"

///< Debug
/*
void getFreeMemory()
{
  printf("Free bytes:");
  itoa(freeMemory(), ShortMessage, 10);
  printf("%s\n",&ShortMessage);
}
*/

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
  if (std::isnan(Number))
    Number = -1.0;
  sprintf(ShortMessage, "%.2f", Number); ///< with 2 digits after the decimal point
  return ShortMessage;
}

char *toText_floatDecimals(float Number)
{
  if (std::isnan(Number))
    Number = -1.0;
  sprintf(ShortMessage, "%.6f", Number); ///< with 6 decimals
  return ShortMessage;
}

char *toText(int Number1, const char *Separator, int Number2)
{
  snprintf(ShortMessage, 32, "%d%s%d", Number1, Separator, Number2);
  return ShortMessage;
}

char *toText(float Number1, const char *Separator, float Number2)
{ ///< function overloading: Same named function, different parameter type
  char Number2Char[MaxWordLength] = "";
  if (std::isnan(Number1))
    Number1 = -1.0;
  if (std::isnan(Number2))
    Number2 = -1.0;
  sprintf(ShortMessage, "%.2f", Number1);
  strcat(ShortMessage, Separator);
  sprintf(Number2Char, "%.2f", Number2);
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
  sprintf(ShortMessage, "%.2f", Temp);
  if (*Metric)
  {
    strcat(ShortMessage, "°C");
  }
  else
  {
    strcat(ShortMessage, "°F");
  }
  return ShortMessage;
}

char *toText_pressure(float Pressure)
{
  sprintf(ShortMessage, "%.2f", Pressure);
  if (*Metric)
  {
    strcat(ShortMessage, "bar");
  }
  else
  {
    strcat(ShortMessage, "psi");
  }
  return ShortMessage;
}

char *toText_weight(float Weight)
{
  sprintf(ShortMessage, "%.2f", Weight);
  if (*Metric)
  {
    strcat(ShortMessage, "kg");
  }
  else
  {
    strcat(ShortMessage, "lbs");
  }
  return ShortMessage;
}

char *toText_percentage(float Number)
{
  ///< static char * ReturnChar = malloc(MaxWordLength * sizeof(char));  ///< allocate memory for every run - need to take care of freeing up the memory  after use
  sprintf(ShortMessage, "%.2f", Number);
  strcat(ShortMessage, "%");
  return ShortMessage;
}

char *toText_percentage(int Number)
{
  itoa(Number, ShortMessage, 10);
  strcat(ShortMessage, "%");
  return ShortMessage;
}

char *toText_rpm(float RPM)
{
  sprintf(ShortMessage, "%.2f", RPM);
  strcat(ShortMessage, "rpm");
  return ShortMessage;
}

char *toText_minute(int Minute)
{
  itoa(Minute, ShortMessage, 10);
  strcat(ShortMessage, "min");
  return ShortMessage;
}

char *toText_second(int Second)
{
  itoa(Second, ShortMessage, 10);
  strcat(ShortMessage, "sec");
  return ShortMessage;
}

char *toText_distance(float Distance)
{
  sprintf(ShortMessage, "%.2f", Distance);
  if (*Metric)
  {
    strcat(ShortMessage, "cm");
  }
  else
  {
    strcat(ShortMessage, "inch");
  }
  return ShortMessage;
}

char *toText_TDS(float TDS)
{
  sprintf(ShortMessage, "%.2f", TDS);
  strcat(ShortMessage, "ppm");
  return ShortMessage;
}

char *toText_onOff(bool Status)
{
  if (Status)
    return (char *)"ON";
  else
    return (char *)"OFF";
}

char *toText_enabledDisabled(bool Status)
{
  if (Status)
    return (char *)"ENABLED";
  else
    return (char *)"DISABLED";
}

const char *toText_yesNo(bool Status)
{
  if (Status)
    return "YES";
  else
    return "NO";
}

const char *toText_onOffDisabled(bool Enabled, bool OnStatus)
{
  if (!Enabled)
    return "DISABLED";
  else
  {
    return toText_onOff(OnStatus);
  }
}

const char *toText_onlineStatus(bool Status)
{
  if (Status)
    return "ONLINE";
  else
    return "OFFLINE";
}

const char *toText_connectedStatus(bool Status)
{
  if (Status)
    return "CONNECTED";
  else
    return "DISCONNECTED";
}

/// Converting text

bool toBool(char *Boolean)
{
  if (strcmp(Boolean, "on") == 0)
    return 1;
  if (strcmp(Boolean, "1") == 0)
    return 1;
  if (strcmp(Boolean, "true") == 0)
    return 1;
  if (strcmp(Boolean, "yes") == 0)
    return 1;
  return 0;
}

int toInt(char *Integer)
{
  return atoi(Integer);
}

float toFloat(char *Float)
{
  return atof(Float);
}

//< State related functions

const char *toText_ACMotorState(ACMotorStates State)
{
  switch (State)
  {
  case ACMotorStates::IDLE:
    return "IDLE";
    break;
  case ACMotorStates::FORWARD:
    return "FORWARD";
    break;
  case ACMotorStates::BACKWARD:
    return "BACKWARD";
    break;
  case ACMotorStates::STOPPING:
    return "STOPPING";
    break;
  default:
    return "?";
    break;
  }
}

const char *toText_waterPumpState(WaterPumpStates State)
{
  switch (State)
  {
  case WaterPumpStates::DISABLED:
    return "DISABLED";
    break;
  case WaterPumpStates::IDLE:
    return "IDLE";
    break;
  case WaterPumpStates::RUNNING:
    return "RUNNING";
    break;
  default:
    return "?";
    break;
  }
}

const char *toText_wasteReservoirStates(WasteReservoirStates State)
{
  switch (State)
  {
  case WasteReservoirStates::FULL:
    return "FULL";
    break;
  case WasteReservoirStates::IDLE:
    return "IDLE";
    break;
  case WasteReservoirStates::RESERVED:
    return "RESERVED";
    break;
  default:
    return "?";
    break;
  }
}

const char *toText_pressurePumpState(PressurePumpStates State)
{
  switch (State)
  {
  case PressurePumpStates::DISABLED:
    return "DISABLED";
    break;
  case PressurePumpStates::IDLE:
    return "IDLE";
    break;
  case PressurePumpStates::PRIMING:
    return "PRIMING";
    break;
  case PressurePumpStates::RUNNING:
    return "RUNNING";
    break;
  case PressurePumpStates::BLOWOFF:
    return "BLOWOFF";
    break;
  case PressurePumpStates::MIX:
    return "MIX";
    break;
  case PressurePumpStates::BYPASSCLOSE:
    return "BYPASSCLOSE";
    break;
  case PressurePumpStates::BYPASSOPEN:
    return "BYPASSOPEN";
    break;
  default:
    return "?";
    break;
  }
}

const char *toText_aeroTankState(AeroTankStates State)
{
  switch (State)
  {
  case AeroTankStates::DISABLED:
    return "DISABLED";
    break;
  case AeroTankStates::IDLE:
    return "IDLE";
    break;
  case AeroTankStates::SPRAY:
    return "SPRAY";
    break;
  case AeroTankStates::STOPSPRAY:
    return "STOPSPRAY";
    break;
  case AeroTankStates::RELEASE:
    return "RELEASE";
    break;
  case AeroTankStates::REFILL:
    return "REFILL";
    break;
  case AeroTankStates::DRAIN:
    return "DRAIN";
    break;
  case AeroTankStates::MIX:
    return "MIX";
    break;
  default:
    return "?";
    break;
  }
}

const char *toText_aeroNoTankState(AeroNoTankStates State)
{
  switch (State)
  {
  case AeroNoTankStates::DISABLED:
    return "DISABLED";
    break;
  case AeroNoTankStates::PRIMING:
    return "PRIMING";
    break;
  case AeroNoTankStates::IDLE:
    return "IDLE";
    break;
  case AeroNoTankStates::SPRAY:
    return "SPRAY";
    break;
  case AeroNoTankStates::RELEASE:
    return "RELEASE";
    break;
  case AeroNoTankStates::MIX:
    return "MIX";
    break;
  default:
    return "?";
    break;
  }
}

const char *toText_hempyState(HempyStates State)
{
  switch (State)
  {
  case HempyStates::DISABLED:
    return "DISABLED";
    break;
  case HempyStates::IDLE:
    return "IDLE";
    break;
  case HempyStates::WATERING:
    return "WATERING";
    break;
  case HempyStates::DRAINING:
    return "DRAINING";
    break;
  default:
    return "?";
    break;
  }
}

const char *toText_lightState(LightStates State)
{
  switch (State)
  {
  case LightStates::TURNEDOFF:
    return "OFF";
    break;
  case LightStates::TURNEDON:
    return "ON";
    break;
  case LightStates::FADEIN:
    return "FADEIN";
    break;
  case LightStates::FADEOUT:
    return "FADEOUT";
    break;
  case LightStates::DIMMED:
    return "DIMMED";
    break;
  default:
    return "?";
    break;
  }
}

//0:OK,-1:TIMEOUT,-2:GENERIC,-3:NO_DATA,-4:NOT_PERMITTED,-5:INVALID_ARG,-6:IO:,-7:BADAUTH,-8:CONNECT_FAILED,-9:INSUFFICIENT_RESOURCES
const char*toText_WiFiConnectResult(int Status)
{
  switch (Status)
  {
  case 0:
    return "OK";
    break;
  case -1:
    return "TIMEOUT";
    break;
  case -2:
    return "GENERIC";
    break;
  case -3:
    return "NODATA";
    break;
  case -4:
    return "NOT_PERMITTED";
    break;
  case -5:
    return "INVALID_ARG";
    break;
  case -6:
    return "IO";
    break;
  case -7:
    return "BADAUTH";
    break;
  case -8:
    return "CONNECT_FAILED";
    break;
  case -9:
    return "INSUFFICIENT_RESOURCES";
    break;
  default:
    return "?";
    break;
  }
}

const char *toText_WiFiStatus(int Status)
{
  switch (Status)
  {
  case 0:
    return "DOWN";
    break;
  case 1:
    return "JOIN";
    break;
  case 2:
    return "NOIP";
    break;
  case 3:
    return "UP";
    break;
  case -1:
    return "FAIL";
    break;
  case -2:
    return "NONET";
    break;
  case -3:
    return "BADAUTH";
    break;
  default:
    return "?";
    break;
  }
}

const char *toText_MqttStatus(int Status)
{
  switch (Status)
  {
  case 0:
    return "ACCEPTED ";
    break;
  case 1:
    return "REFUSED_PROTOCOL";
    break;
  case 2:
    return "REFUSED_IDENTIFIER";
    break;
  case 3:
    return "REFUSED_UNAUTHORIZED";
    break;
  case 4:
    return "NOT_PERMITTED";
    break;
  case 5:
    return "INVALID_ARG";
    break;
  case 256:
    return "DISCONNECTED";
    break;
  case 257:
    return "TIMEOUT";
    break;
  default:
    return "?";
    break;
  }
}