/*! \file
 *  \brief     Test setting the integrated Real Time Clock using NTP via LWIP
 *  \details   DOES NOT WORK! updateRTC never gets called, even though the LWIP SNTP client gets the result from the NTP server
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/raspberry-pi-pico-w
 *  \version   4.20
 */

#pragma once

#define WIFI_SSID "GboxNet"                     // UPDATE THIS
#define WIFI_PASSWORD "SuperSecretPassword"     // UPDATE THIS
#define RUN_FREERTOS_ON_CORE 0
#define TASK_PRIORITY_L1 (tskIDLE_PRIORITY + 1UL) // Lower means less important
#define TASK_PRIORITY_L2 (tskIDLE_PRIORITY + 2UL) // Higher means more important
#define TASK_PRIORITY_L3 (tskIDLE_PRIORITY + 3UL) // Higher means more important
#define SNTP_STARTUP_DELAY 0
#define SNTP_SET_SYSTEM_TIME_NTP(sec,frac) updateRTC(sec,frac)

#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"
#include "lwip/apps/sntp.h" ///< Could not get 
#include "pico/cyw43_arch.h"
#include "pico/util/datetime.h"
#include <time.h>
#include "FreeRTOS.h"
#include "task.h"


// MQTT client settings structure
struct NtpClientSettings {
    char Name[32];             ///< Must be unique
    char NtpServerDNS[32];     ///< NTP server DNS name, "" to use MqttServerIP instead
    char NtpServerIP[32];      ///< NTP server IP. Used when MqttServerDNS is empty, or the DNS lookup fails
    uint16_t NtpServerPort;    ///< NTP server Port
    int8_t TimeZoneDifference; ///<  UTC time and current timezone difference
    uint32_t TimeoutSeconds;   ///< Duration in seconds that an NTP client waits for a response from an NTP server
};
  struct NtpClientSettings NTPServer1 = {.Name = "NTP1", .NtpServerDNS = "pool.ntp.org", .NtpServerIP = "192.168.1.100", .NtpServerPort = 123, .TimeZoneDifference = -7, .TimeoutSeconds = 15};  ///< Designated initializers (C99 feature)

// Global variable declaration
datetime_t CurrentTime;
char CurrentTime_buffer[128];

// Function declarations
int main(); ///< Entry point. Load settings, create tasks, and starts FreeRTOS scheduler
extern "C" void updateRTC(u32_t sec,u32_t frac) __attribute__((used));  ///< Callback for LWIP SNTP, called when the current time is retrieved from an NTP server.  the __attribute__((used)) informs the compiler that the function is used, even if it appears to be unused based on local analysis
void getRTC(); ///< Prints the current time to stdout
void wifiTask(void *pvParameters); ///< Initialize WiFi, periodically check the connection, and reconnect if needed
void connectWiFi(); ///< Connect to a WiFi network
void rtcTask(void *pvParameters); ///< Use NTP to update the Real Time Clock
void heartbeatTask(void *pvParameters); ///< Blink built-in LED on Pico W - Requires WiFi initialization cyw43_arch_init() , since the LED is connected to the WiFi controller
bool DnsLookup(char *DnsName, ip_addr_t *ResultIP);
void DnsLookupResult(const char *Hostname, const ip_addr_t *FoundIP, void *ResultIP);

// Static variables
static bool ledStatus = false; ///< Track the current state of the built-in LED
static bool dnsLookupInProgress = false; ///< true: DNS lookup in progress
static bool dnsLookupSuccess = false;    ///< true: DNS lookup resulted in an IP