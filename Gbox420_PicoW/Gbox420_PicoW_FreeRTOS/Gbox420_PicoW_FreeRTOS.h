#pragma once

#include "pico/stdlib.h"
#include "hardware/watchdog.h" // Watchdog to auto-reboot in case of an error
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "Settings.h"
#include "Components/Helpers.h"
#include "Components/Sound.h"

#define RUN_FREERTOS_ON_CORE 0
#define TASK_PRIORITY_L1 (tskIDLE_PRIORITY + 1UL) // Lower means less important
#define TASK_PRIORITY_L2 (tskIDLE_PRIORITY + 2UL) // Higher means more important
#define TASK_PRIORITY_L3 (tskIDLE_PRIORITY + 3UL) // Higher means more important
#define NTP_MSG_LEN 48
#define NTP_DELTA 2208988800 // seconds between 1 Jan 1900 and 1 Jan 1970
#define NTP_RESEND_TIME (10 * 1000)

int main(); ///< Entry point. Loads settings, create tasks and starts FreeRTOS scheduler

void watchdogTask(void *pvParameters);                                                                         ///< Initialize watchdog and periodically pet it - Crash detection and reboot
void wifiTask(void *pvParameters);                                                                             ///< Initialize WiFi, periodically check the connection and reconnect if needed
void connectWiFi();                                                                                            ///< Connect to a WiFi network
void ntpTask(void *pvParameters);                                                                              ///< Use NTP to update the Real Time Clock
void ntpRequest();                                                                                             ///< Make an NTP request
static void ntpReceived(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port); ///< NTP data received
void heartbeatTask(void *pvParameters);                                                                        ///< Blink built-in LED on Pico W - Requires WiFi initialization cyw43_arch_init() , since the LED is connected to the WiFi controller
Settings *GboxSettings;                                                                                        ///< This object will store the settings loaded from the Settings.h. //TODO: Find a solution to Pico W not having EEPROM
ip_addr_t NtpServerIP;                                                                                         ///< Store the resolved IP address of the NTP server
struct udp_pcb *NtpPcb;                                                                                        ///< UDP control block for NTP
datetime_t CurrentTime;                                                                                        ///< Used when getting/setting the Real Time Clock
char CurrentTimeText[MaxShotTextLength];                                                                       ///< Store current time in text format
char LongMessage[MaxLongTextLength] = "";  ///< Temp storage for assembling long messages (REST API, MQTT reporting)
char ShortMessage[MaxShotTextLength] = ""; ///< Temp storage for assembling short messages (Log entries, Error messages)
bool *Debug;                                                                                                   ///< True - Turns on extra debug messages on the Serial Output
bool *Metric;                                                                                                  ///< True - Use metric units, False - Use imperial units
bool ledStatus = false;                                                                                        ///< Track the current state of the built-in LED