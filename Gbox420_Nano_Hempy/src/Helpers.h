#pragma once

///This is a collection of common functions that every component can use
///Needs to be included in the module's header (.h) file:  #include "420Common.h"
///During conversion the result is always copied into the ShortMessage global char array.

#include "Arduino.h"           
#include "TimeLib.h"           ///Keeping track of time
#include "MemoryFree.h"        ///checking remaining memory - only for debugging
#include "avr/wdt.h"           ///Watchdog timer
#include "../SerialLog.h"
#include "../Settings.h"       ///for storing/reading defaults
#include "RollingAverage.h" ///for smoothing sensor readings, average of last readings


extern HardwareSerial &ArduinoSerial;
extern HardwareSerial &ESPSerial;
extern char CurrentTime[MaxWordLength];
extern char ShortMessage[MaxShotTextLength];
extern char LongMessage[MaxLongTextLength];
extern Settings * ModuleSettings;
extern bool *Debug;
extern bool *Metric;

///Class specific variables
void getFreeMemory();
float convertBetweenTempUnits(float);
float convertBetweenPressureUnits(float);
char *toText(const __FlashStringHelper *);
char *toText(int);
char *toText(long);
char *toText(unsigned int);
char *toText(float); ///function overloading: Same named function, different parameter type
char *toText(int Number1, int Number2, const char *Separator);
char *toText(float Number1, float Number2, const char *Separator);
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
char *toText_pumpState(PumpStates State);
char *toText_minute(int Minute);
char *toText_second(int Second);
char *toText_distance(float Distance);