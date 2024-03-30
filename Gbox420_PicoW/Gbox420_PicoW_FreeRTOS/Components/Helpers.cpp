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

// Current time from local RTC
char *getCurrentTime(bool PrintToSerial)
{
  datetime_t CurrentDateTime;
  rtc_get_datetime(&CurrentDateTime);
  datetime_to_str(ShortMessage, MaxShotTextLength, &CurrentDateTime);
  if (PrintToSerial)
  {
    printf("%s\n", ShortMessage);
  }
  return ShortMessage;
}

///< DNS client

/// @brief Lookup a DNS name while blocking processing until a result is received. Cannot be called from a timer interrupt like run1sec,run5sec,run1min,report
/// @param DnsName Pointer to char containing the name to lookup
/// @param ResultIP Pointer to the ip_addr_t object where the lookup result will be stored
/// @return true: Lookup successful, false: lookup failed
bool DnsLookup(char *DnsName, ip_addr_t *ResultIP)
{
  dnsLookupSuccess = false;
  dnsLookupInProgress = true;
  printf("   Looking up IP for %s...", DnsName);
  err_t err = dns_gethostbyname(DnsName, ResultIP, DnsLookupResult, ResultIP);
  if (err == ERR_OK) // DNS name found in cache and loaded into ResultIP
  {
    printf("Found cached address");
    return true;
  }
  absolute_time_t Timeout = make_timeout_time_ms(15000); // 15sec from now
  while (dnsLookupInProgress)                            // Waiting for the DNS lookup to finish and DnsLookupResult callback to trigger
  {
    if (get_absolute_time() > Timeout)
    {
      printf("DNS lookup timeout\n");
      return false;
    }
    vTaskDelay(500);
  }
  return dnsLookupSuccess;
}

/// @brief Callback when DNS lookup is finished
/// @param Hostname The DNS name
/// @param FoundIP The IP for the Name
/// @param ResultIP
void DnsLookupResult(const char *Hostname, const ip_addr_t *FoundIP, void *ResultIP) // DNS lookup callback
{
  if (FoundIP)
  {
    printf("Found address: %s\n", ipaddr_ntoa(FoundIP));
    ip_addr_copy(*(ip_addr_t *)ResultIP, *FoundIP);
    dnsLookupSuccess = true;
  }
  else
  {
    printf("DNS lookup failed\n");
  }
  dnsLookupInProgress = false;
}

/// @brief Lookup a DNS name in the background, result will be copied to ResultIP
/// @param DnsName Containing the name to lookup
/// @param ResultIP ip_addr_t object where the lookup result will be stored
/// @param DataCallback Callback function for when DNS result is ready
/// @return true: IP already available, false: Lookup started, no cached IP found
// Implementation of the wrapper function
bool DnsLookup_Async(char *DnsName, ip_addr_t *ResultIP, std::function<void(ip_addr_t *)> DataCallback)
{
  dnsLookupSuccess = false;
  dnsLookupInProgress = true;
  printf("   Looking up IP for %s\n", DnsName);
  DnsCallbackData *callbackData = new DnsCallbackData{DataCallback};                     // Create an instance of the callback data structure
  err_t err = dns_gethostbyname(DnsName, ResultIP, DnsLookupResult_Async, callbackData); // Initiate the DNS lookup, passing the callback function and its associated data
  if (err == ERR_OK)
  {                      // ERR_OK: DNS name found in cache and loaded into ResultIP
    delete callbackData; // Clean up callback data if the lookup is already completed
    return true;
  }
  return false;
}

void DnsLookupResult_Async(const char *Hostname, const ip_addr_t *FoundIP, void *DataCallback) // DNS lookup callback
{
  DnsCallbackData *DnsCallback = static_cast<DnsCallbackData *>(DataCallback);
  if (FoundIP)
  {

    printf("%s address: %s\n", Hostname, ipaddr_ntoa(FoundIP));
    ip_addr_t ResultIP;
    ip_addr_copy(ResultIP, *FoundIP);
    DnsCallback->callback(&ResultIP);
  }
  else
  {
    printf("%s lookup failed\n", Hostname);
  }
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

char *toText_yesNo(bool Status)
{
  if (Status)
    return (char *)"YES";
  else
    return (char *)"NO";
}

char *toText_enabledDisabled(bool Status)
{
  if (Status)
    return (char *)"ENABLED";
  else
    return (char *)"DISABLED";
}

char *toText_onOff(bool Status)
{
  if (Status)
    return (char *)"ON";
  else
    return (char *)"OFF";
}

char *toText_onOffDisabled(bool Enabled, bool OnStatus)
{
  if (!Enabled)
    return (char *)"DISABLED";
  else
  {
    return toText_onOff(OnStatus);
  }
}

char *toText_onlineStatus(bool Status)
{
  if (Status)
    return (char *)"ONLINE";
  else
    return (char *)"OFFLINE";
}

char *toText_connectedStatus(bool Status)
{
  if (Status)
    return (char *)"CONNECTED";
  else
    return (char *)"DISCONNECTED";
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

char *toText_ACMotorState(ACMotorStates State)
{
  switch (State)
  {
  case ACMotorStates::IDLE:
    return (char *)"IDLE";
    break;
  case ACMotorStates::FORWARD:
    return (char *)"FORWARD";
    break;
  case ACMotorStates::BACKWARD:
    return (char *)"BACKWARD";
    break;
  case ACMotorStates::STOPPING:
    return (char *)"STOPPING";
    break;
  default:
    return (char *)"?";
    break;
  }
}

char *toText_waterPumpState(WaterPumpStates State)
{
  switch (State)
  {
  case WaterPumpStates::DISABLED:
    return (char *)"DISABLED";
    break;
  case WaterPumpStates::IDLE:
    return (char *)"IDLE";
    break;
  case WaterPumpStates::RUNNING:
    return (char *)"RUNNING";
    break;
  default:
    return (char *)"?";
    break;
  }
}

char *toText_wasteReservoirStates(WasteReservoirStates State)
{
  switch (State)
  {
  case WasteReservoirStates::FULL:
    return (char *)"FULL";
    break;
  case WasteReservoirStates::IDLE:
    return (char *)"IDLE";
    break;
  case WasteReservoirStates::RESERVED:
    return (char *)"RESERVED";
    break;
  default:
    return (char *)"?";
    break;
  }
}

char *toText_pressurePumpState(PressurePumpStates State)
{
  switch (State)
  {
  case PressurePumpStates::DISABLED:
    return (char *)"DISABLED";
    break;
  case PressurePumpStates::IDLE:
    return (char *)"IDLE";
    break;
  case PressurePumpStates::PRIMING:
    return (char *)"PRIMING";
    break;
  case PressurePumpStates::RUNNING:
    return (char *)"RUNNING";
    break;
  case PressurePumpStates::BLOWOFF:
    return (char *)"BLOWOFF";
    break;
  case PressurePumpStates::MIX:
    return (char *)"MIX";
    break;
  case PressurePumpStates::BYPASSCLOSE:
    return (char *)"BYPASSCLOSE";
    break;
  case PressurePumpStates::BYPASSOPEN:
    return (char *)"BYPASSOPEN";
    break;
  default:
    return (char *)"?";
    break;
  }
}

char *toText_aeroTankState(AeroTankStates State)
{
  switch (State)
  {
  case AeroTankStates::DISABLED:
    return (char *)"DISABLED";
    break;
  case AeroTankStates::IDLE:
    return (char *)"IDLE";
    break;
  case AeroTankStates::SPRAY:
    return (char *)"SPRAY";
    break;
  case AeroTankStates::STOPSPRAY:
    return (char *)"STOPSPRAY";
    break;
  case AeroTankStates::RELEASE:
    return (char *)"RELEASE";
    break;
  case AeroTankStates::REFILL:
    return (char *)"REFILL";
    break;
  case AeroTankStates::DRAIN:
    return (char *)"DRAIN";
    break;
  case AeroTankStates::MIX:
    return (char *)"MIX";
    break;
  default:
    return (char *)"?";
    break;
  }
}

char *toText_aeroNoTankState(AeroNoTankStates State)
{
  switch (State)
  {
  case AeroNoTankStates::DISABLED:
    return (char *)"DISABLED";
    break;
  case AeroNoTankStates::PRIMING:
    return (char *)"PRIMING";
    break;
  case AeroNoTankStates::IDLE:
    return (char *)"IDLE";
    break;
  case AeroNoTankStates::SPRAY:
    return (char *)"SPRAY";
    break;
  case AeroNoTankStates::RELEASE:
    return (char *)"RELEASE";
    break;
  case AeroNoTankStates::MIX:
    return (char *)"MIX";
    break;
  default:
    return (char *)"?";
    break;
  }
}

char *toText_hempyState(HempyStates State)
{
  switch (State)
  {
  case HempyStates::DISABLED:
    return (char *)"DISABLED";
    break;
  case HempyStates::IDLE:
    return (char *)"IDLE";
    break;
  case HempyStates::WATERING:
    return (char *)"WATERING";
    break;
  case HempyStates::DRAINING:
    return (char *)"DRAINING";
    break;
  default:
    return (char *)"?";
    break;
  }
}

char *toText_lightState(LightStates State)
{
  switch (State)
  {
  case LightStates::TURNEDOFF:
    return (char *)"OFF";
    break;
  case LightStates::TURNEDON:
    return (char *)"ON";
    break;
  case LightStates::FADEIN:
    return (char *)"FADEIN";
    break;
  case LightStates::FADEOUT:
    return (char *)"FADEOUT";
    break;
  case LightStates::DIMMED:
    return (char *)"DIMMED";
    break;
  default:
    return (char *)"?";
    break;
  }
}