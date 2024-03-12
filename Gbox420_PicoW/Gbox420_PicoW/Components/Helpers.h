/*! \file
 *  \brief     Set of common functions any class can use
 *  \details   During conversion the result is always copied into the ShortMessage global char array
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/
 *  \version   4.20
 */

#pragma once

// #include "MemoryFree.h" // Checking remaining memory
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
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

extern char CurrentTime[MaxWordLength];
extern char ShortMessage[MaxShotTextLength];
extern char LongMessage[MaxLongTextLength];
extern Settings *ModuleSettings;
extern bool *Debug;
extern bool *Metric;

// Class specific variables
// void getFreeMemory();
// Query current time from local RTC
char *getCurrentTime(bool PrintToSerial);
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
char *toText_yesNo(bool Status);
char *toText_enabledDisabled(bool Status);
char *toText_onlineStatus(bool Status);
char *toText_aeroTankState(AeroTankStates State);
char *toText_aeroNoTankState(AeroNoTankStates State);
char *toText_ACMotorState(ACMotorStates State);
char *toText_wasteReservoirStates(WasteReservoirStates State);
char *toText_waterPumpState(WaterPumpStates State);
char *toText_pressurePumpState(PressurePumpStates State);
char *toText_hempyState(HempyStates State);
char *toText_lightState(LightStates State);
char *toText_minute(int Minute);
char *toText_second(int Second);
char *toText_distance(float Distance);
char *toText_TDS(float TDS);
char *toText_rpm(float RPM);
bool toBool(char *Boolean);
int toInt(char *Integer);
float toFloat(char *Float);