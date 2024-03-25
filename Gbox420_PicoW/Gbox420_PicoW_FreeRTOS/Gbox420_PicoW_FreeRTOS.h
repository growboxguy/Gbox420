#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
//#include "core_sntp_client.h"
#include "Settings.h"
#include "Components/Helpers.h"
#include "Components/NtpClient.h"
#include "Components/Sound.h"


#define RUN_FREERTOS_ON_CORE 0
#define TASK_PRIORITY_L1 (tskIDLE_PRIORITY + 1UL) // Lower means less important
#define TASK_PRIORITY_L2 (tskIDLE_PRIORITY + 2UL) // Higher means more important
#define TASK_PRIORITY_L3 (tskIDLE_PRIORITY + 3UL) // Higher means more important

// Initialize WiFi and Connect to local network
void connectWiFi();
void wifiChecker(void *pvParameters);
void rtcChecker(void *pvParameters);

Settings *GboxSettings;    ///< This object will store the settings loaded from the Settings.h. //TODO: Find a solution to Pico W not having EEPROM
NtpClient *NtpClient1;

bool *Debug;                               ///< True - Turns on extra debug messages on the Serial Output
bool *Metric;                              ///< True - Use metric units, False - Use imperial units


///< Blink built-in LED on Pico W - Requires WiFi initialization cyw43_arch_init() , since the LED is connected to the WiFi controller
bool ledStatus = false;
void heartbeat(void *pvParameters);
int main();