/*!
*  \brief     Gbox420 port for Raspberry Pico W (UNDER DEVELOPMENT)
*  \details   Currently, only the core of Gbox420 is ported:
*              - Connecting to a WiFi network defined in Settings.h (WIFI_SSID,WIFI_PASSWORD), and periodically checking if the connection is up (WIFI_TIMEOUT), reconnecting if needed
*              - Syncing the Pico's built-in Real-Time Clock (RTC) using an NTP query and periodically re-syncing
*              - Connecting to an MQTT server defined in Settings.h (MqttClientSettings MqttServer1) and subscribing to a topic and all of it's subtopics. The MQTT server connection is regularly monitored for reconnection
*              - DNS lookup for finding the NTP and MQTT servers by name (MqttServerDNS, NtpServerDNS)
*              - Onboard LED control: Always on: WiFi connection attempt in progress, Rapid (0.25sec) blinking: MQTT server not connected, Slow blinking (1sec): MQTT server connected
*              - UART and USB logging, check out UartOutputSample.txt
*              - Built on top of FreeRTOS (using tasks and timers currently, will include semaphores too)
*             Missing:
*              - Everything related to monitoring a grow tent (Modules and Components)
*              - Self-hosted website
*              - Sound support via piezo buzzer
*              - EEPROM : Since the Pico W lacks EEPROM to retain settings changes between reboots, defaults are loaded from Settings.h upon every boot
*  \author    GrowBoxGuy
*  \version   4.20

 */

#pragma once

#include "pico/stdlib.h"
#include "hardware/watchdog.h" // Watchdog to auto-reboot in case of an error
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "Settings.h"
#include "MqttClient.h"
#include "Components/Helpers.h"
#include "Components/GboxModule.h"
#include "Components/HempyModule.h"

#define RUN_FREERTOS_ON_CORE 0
#define TASK_PRIORITY_L1 (tskIDLE_PRIORITY + 1UL) // Lower means less important
#define TASK_PRIORITY_L2 (tskIDLE_PRIORITY + 2UL) // Higher means more important
#define TASK_PRIORITY_L3 (tskIDLE_PRIORITY + 3UL) // Higher means more important

int main();                                                                                                    ///< Entry point. Loads settings, create tasks and starts FreeRTOS scheduler
void watchdogTask(void *pvParameters);                                                                         ///< Initialize watchdog and periodically pet it - Crash detection and reboot
void hempyTask(void *pvParameters);                                                                            ///< Hempy module refresh task
void serialReadTask(void *pvParameters);                                                                       ///< Read incoming stdin messages and try to process then as MQTT commands
void run1Sec(TimerHandle_t xTimer);                                                                            ///< Runs every 1 sec
void run5Sec(TimerHandle_t xTimer);                                                                            ///< Runs every 5 sec
void run1Min(TimerHandle_t xTimer);                                                                            ///< Runs every 1 min
void run30Min(TimerHandle_t xTimer);                                                                           ///< Runs every 30 min
void connectivityTask(void *pvParameters);                                                                     ///< Initialize WiFi, periodically check the connection and reconnect if needed. Use NTP to update the Real Time Clock
uint8_t ConnectivityCounter = WIFI_TIMEOUT;                                                                    // Count the seconds since the last WiFi connectivity check
void heartbeat();                                                                                              ///< Controls the onboard LED, blink every sec: MQTT connected, blink every 0,5sec: MQTT disconnected. !! Makes the caller task delay a total of 1 sec !!
bool connectWiFi();                                                                                            ///< Connect to a WiFi network
bool dnsLookup(char *DnsName, ip_addr_t *ResultIP);                                                            ///< Start a DNS lookup for DnsName, update ResultIP with the result. Returns true if DNS lookup was successful
void dnsLookupResult(const char *Hostname, const ip_addr_t *ResultIP, void *Arg);                              ///< Callback with the lookup result
void rtcInit();                                                                                                //< Initialize the Real Time Clock and set a stating date
char *rtcGetCurrentTime(bool PrintToSerial);                                                                   ///< Query current time from local RTC
uint8_t NtpSynced = 0;                                                                                         ///< 0: Not synced with an NTP server. NtpSynced is uint8_t, overflows after 255 checks -> Forces an NTP update every hour with WIFI_TIMER set to 15sec. If NtpSynced is changed to uint16_t the sync delay is ~11 days
void ntpRequest();                                                                                             ///< Make an NTP request
static void ntpReceived(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port); ///< NTP data received
void mqttDataReceived(char *SubTopicReceived, char *DataReceived);                                             ///< Callback when MQTT data is received on a subscribed topic
void mqttPublish(const char *Topic, const char *Data);                                                         ///< Publish data to an MQTT topic
Settings *GboxSettings;                                                                                        ///< This object will store the settings loaded from the Settings.h. //TODO: Find a solution to Pico W not having EEPROM
MqttClient *MqttClientDefault = nullptr;                                                                       ///< Pointer to MQTT handler
GboxModule *GboxModule1;                                                                                       ///< Core module, provides Sound feedback
HempyModule *HempyModule1;                                                                                     ///< Represents a Hempy module with all of its components
ip_addr_t NtpServerIP;                                                                                         ///< Store the resolved IP address of the NTP server
struct udp_pcb *NtpPcb;                                                                                        ///< UDP control block for NTP
datetime_t CurrentTime;                                                                                        ///< Used when getting/setting the Real Time Clock
char CurrentTimeText[MaxShotTextLength];                                                                       ///< Store current time in text format
char LongMessage[MaxLongTextLength] = "";                                                                      ///< Temp storage for assembling long messages (REST API, MQTT reporting)
char ShortMessage[MaxShotTextLength] = "";                                                                     ///< Temp storage for assembling short messages (Log entries, Error messages)
bool *Debug;                                                                                                   ///< True - Turns on extra debug messages on the Serial Output
bool *Metric;                                                                                                  ///< True - Use metric units, False - Use imperial units
bool ledStatus = true;                                                                                         ///< Track the current state of the built-in LED