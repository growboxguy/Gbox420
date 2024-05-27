/*! \file
 *  \brief     Set of common functions any class can use
 *  \details   During conversion the result is always copied into the ShortMessage global char array
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/
 *  \version   4.20
 */

#pragma once

// #include "MemoryFree.h" // Checking remaining memory
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "hardware/rtc.h"
#include "pico/cyw43_arch.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"
#include "pico/util/datetime.h"
#include <functional>
#include "FreeRTOS.h"
#include "task.h"
#include "../Settings.h" // Storing/reading defaults

///< State machine - Defining possible states

enum class ACMotorStates
{
  IDLE,
  FORWARD,
  BACKWARD,
  STOPPING
};

enum class WasteReservoirStates
{
  FULL,
  IDLE,
  RESERVED
};

enum class WaterPumpStates
{
  DISABLED,
  IDLE,
  RUNNING
};

enum class HempyStates
{
  DISABLED,
  IDLE,
  WATERING,
  DRAINING
};

enum class PressurePumpStates
{
  DISABLED,
  IDLE,
  PRIMING,
  RUNNING,
  BLOWOFF,
  MIX,
  BYPASSCLOSE,
  BYPASSOPEN
};

enum class AeroTankStates
{
  DISABLED,
  IDLE,
  SPRAY,
  STOPSPRAY,
  RELEASE,
  REFILL,
  DRAIN,
  MIX
};

enum class AeroNoTankStates
{
  DISABLED,
  IDLE,
  PRIMING,
  SPRAY,
  RELEASE,
  MIX
};

///< Light controller with PWM dimming
enum class LightStates
{
  TURNEDOFF,
  TURNEDON,
  FADEIN,
  FADEOUT,
  DIMMED
};

extern char ShortMessage[MaxShotTextLength];
extern char LongMessage[MaxLongTextLength];
extern Settings *GboxSettings;
extern bool *Debug;
extern bool *Metric;

///< DNS client
struct DnsCallbackData
{ ///< Stucture that holds an object function pointer (Can be passed to LWIP DNS as a void* pointer)
  std::function<void(ip_addr_t *)> callback;
};
static bool __attribute__((unused)) dnsLookupInProgress = false; ///< true: DNS lookup in progress
static bool __attribute__((unused)) dnsLookupSuccess = false;    ///< true: DNS lookup resulted in an IP

// Class specific variables
// void getFreeMemory();
extern char *rtcGetCurrentTime(bool PrintToSerial);                                      ///< Query current time from local RTC
extern bool dnsLookup(char *DnsName, ip_addr_t *ResultIP);                               ///< Start a DNS lookup for DnsName, update ResultIP with the result. Returns true if DNS lookup was successful
extern void dnsLookupResult(const char *Hostname, const ip_addr_t *ResultIP, void *Arg); ///< Callback with the lookup result

bool isThisForMe(char const *Name, char const *Command); ///< Checks if Command starts with the Name of the instance followed by _  Example: Object name Sound1, incoming command: Sound1_Ee will return true. The remaining command "Ee" will be copied to ShortMessage buffer
float convertBetweenTempUnits(float);
float convertBetweenPressureUnits(float);
char *toText(int);
char *toText(long);
char *toText(unsigned int);
char *toText(float); ///< function overloading: Same named function, different parameter type
char *toText(int Number1, const char *Separator, int Number2);
char *toText(float Number1, const char *Separator, float Number2);
char *toText_floatDecimals(float Number);
char *toText_time(uint8_t Hour, uint8_t Minute);
char *toText_temp(float Temp);
char *toText_pressure(float Pressure);
char *toText_weight(float Weight);
char *toText_percentage(float Humidity);
char *toText_onOff(bool Status);
char *toText_enabledDisabled(bool Status);
const char *toText_yesNo(bool Status);
const char *toText_onlineStatus(bool Status);
const char *toText_connectedStatus(bool Status);
const char *toText_aeroTankState(AeroTankStates State);
const char *toText_aeroNoTankState(AeroNoTankStates State);
const char *toText_ACMotorState(ACMotorStates State);
const char *toText_wasteReservoirStates(WasteReservoirStates State);
const char *toText_waterPumpState(WaterPumpStates State);
const char *toText_pressurePumpState(PressurePumpStates State);
const char *toText_hempyState(HempyStates State);
const char *toText_lightState(LightStates State);
char *toText_minute(int Minute);
char *toText_second(int Second);
char *toText_distance(float Distance);
char *toText_TDS(float TDS);
char *toText_rpm(float RPM);
const char *toText_WiFiConnectResult(int Status);
const char *toText_WiFiStatus(int Status);
const char *toText_MqttStatus(int Status);
bool toBool(char *Boolean);
int toInt(char *Integer);
float toFloat(char *Float);