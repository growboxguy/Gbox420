#pragma once

#include "pico/stdlib.h"
#include "hardware/watchdog.h" // Watchdog to auto-reboot in case of an error
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "Settings.h"
#include "Components/Helpers.h"
#include "Components/NtpClient.h"
#include "Components/Sound.h"

#define RUN_FREERTOS_ON_CORE 0
#define TASK_PRIORITY_L1 (tskIDLE_PRIORITY + 1UL) // Lower means less important
#define TASK_PRIORITY_L2 (tskIDLE_PRIORITY + 2UL) // Higher means more important
#define TASK_PRIORITY_L3 (tskIDLE_PRIORITY + 3UL) // Higher means more important

int main(); ///< Entry point. Loads settings, create tasks and starts FreeRTOS scheduler
void watchdogTask(void *pvParameters); ///< Initialize watchdog and periodically pet it - Crash detection and reboot
void wifiTask(void *pvParameters); ///< Initialize WiFi, periodically check the connection and reconnect if needed
void connectWiFi(); ///< Connect to a WiFi network
void rtcTask(void *pvParameters); ///< Use NTP to update the Real Time Clock
void heartbeatTask(void *pvParameters); ///< Blink built-in LED on Pico W - Requires WiFi initialization cyw43_arch_init() , since the LED is connected to the WiFi controller
Settings *GboxSettings;             ///< This object will store the settings loaded from the Settings.h. //TODO: Find a solution to Pico W not having EEPROM
NtpClient *NtpClient1; ///< TODO: Add NTP support using LWIP SNMP
bool *Debug;            ///< True - Turns on extra debug messages on the Serial Output
bool *Metric;           ///< True - Use metric units, False - Use imperial units
bool ledStatus = false; ///< Track the current state of the built-in LED