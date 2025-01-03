/*! \file
 *  \brief     Gbox420 port for Pico W - Development in progress
 *  \details
 *  \attention
 *             update the WIFI_SSID and WIFI_PASSWORD in Settings.h
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/raspberry-pi-pico-w
 *  \version   4.20
 */

#pragma GCC diagnostic ignored "-Wstringop-truncation" // Surpress build warning of potential truncation of a string NULL terminator. During string operations the size is pre-defined in Settings.h: MaxWordLength/MaxShotTextLength/MaxLongTextLength
#include "hardware/rtc.h"                              // Real Time Clock for storing current time (Updated over NTP)
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "Settings.h" // Settings for every component
#include "Components/Helpers.h"
#include "Components/NtpClient.h"
#include "Components/Sound.h"
#include "Components/HempyModule.h" // Represents a complete box with all feautres

// Global variable initialization
char LongMessage[MaxLongTextLength] = "";  ///< Temp storage for assembling long messages (REST API, MQTT reporting)
char ShortMessage[MaxShotTextLength] = ""; ///< Temp storage for assembling short messages (Log entries, Error messages)
bool &Debug = *new bool;                                  ///< True - Turns on extra debug messages on the Serial Output
bool &Metric = *new bool;                              ///< True - Use metric units, False - Use imperial units

// Component initialization/declaration
/*ELClient ESPLink(&ESPSerial);             ///< ESP-link. Both SLIP and debug messages are sent to ESP over the ESP Serial link
ELClientWebServer WebServer(&ESPLink);    ///< ESP-link - WebServer API
ELClientCmd ESPCmd(&ESPLink);             ///< ESP-link - Helps getting the current time from the internet using NTP
ELClientRest PushingBoxRestAPI(&ESPLink); ///< ESP-link - REST API
ELClientMqtt MqttAPI(&ESPLink);           ///< ESP-link - MQTT protocol for sending and receiving messages
*/
Settings *GboxSettings;    ///< This object will store the settings
NtpClient *NtpClient1;     ///< Pointer to
HempyModule *HempyModule1; ///< Represents a Grow Box with all components (LED lights, DHT sensors, Power sensor, Hempy Buckets, Water pants..etc)

// Initialize WiFi and Connect to local network
void initializeWiFi()
{
  printf("Initializing WiFi...");
  if (cyw43_arch_init())
  {
    printf("failed\n");
  }
  else
  {
    cyw43_arch_enable_sta_mode(); // Enables Wi-Fi STA (Station) mode
    printf("Connecting to %s", WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 7000)) // Max 7sec
    {
      printf("...failed\n");
    }
    else
      printf("...done\n");
  }
}

void checkWiFi()
{
  printf("WiFi status: %d\n", cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA));
  if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) // Returns the status of the wifi link: CYW43_LINK_DOWN(0)-link is down,CYW43_LINK_JOIN(1)-Connected to WiFi,CYW43_LINK_NOIP(2)-Connected to WiFi, but no IP address,CYW43_LINK_UP  (3)-Connect to WiFi with an IP address,CYW43_LINK_FAIL(-1)-Connection failed,CYW43_LINK_NONET(-2)-No matching SSID found (could be out of range, or down),CYW43_LINK_BADAUTH(-3)-Authentication failure
  {
    cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK); // Try connecting again in the background
    printf("Reconnecting WiFi: %s\n", WIFI_SSID);
  };
}

///< Blink built-in LED on Pico W - Requires running cyw43_arch_init() before calling it!
void heartBeat()
{
  static bool ledStatus;
  ledStatus = !ledStatus;
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, ledStatus);
}

///< This function gets called infinitely at a rate defined by repeating_timer
bool run1sec(struct repeating_timer *t)
{
  watchdog_update(); ///< Pet watchdog
  heartBeat();       ///< Blinks built-in led
  HempyModule1->run1sec();
  return true; // true to continue repeating, false to stop.
}

///< This function gets called infinitely at a rate defined by repeating_timer
bool run5sec(struct repeating_timer *t)
{
  watchdog_update();    ///< Pet watchdog
  getCurrentTime(true); // Prints current time
  HempyModule1->run5sec();
  return true; // true to continue repeating, false to stop.
}

///< This function gets called infinitely every minute
bool run1min(struct repeating_timer *t)
{
  watchdog_update(); ///< Pet watchdog
  checkWiFi();  ///< Ch
  HempyModule1->run1min();
  NtpClient1->run1min();
  return true; // true to continue repeating, false to stop.
}

///< This function gets called infinitely every minute
bool run30min(struct repeating_timer *t)
{
  watchdog_update(); ///< Pet watchdog
  NtpClient1->run30min();
  return true; // true to continue repeating, false to stop.
}

int main()
{
  stdio_init_all();
  if (watchdog_caused_reboot())
  {
    printf("\nRebooted by Watchdog!\n");
  }
  else
  {
    printf("\nClean boot\n");
  }
  printf("\nGbox420 initializing\n");

  // Loading settings from EEPROM
  GboxSettings = loadSettings();
  Debug = &GboxSettings->Debug;
  Metric = &GboxSettings->Metric;

  initializeWiFi();

  // Create the Module objects
  NtpClient1 = new NtpClient(&GboxSettings->NTPServer1);                     // TODO: Auto NTP update time every 24h
  HempyModule1 = new HempyModule(&GboxSettings->HempyModule1, GboxSettings); ///< This is the object representing an entire Grow Box with all components in it. Receives its settings loaded from Settings.h

  watchdog_enable(0x7fffff, 1); // Maximum of 0x7fffff, which is approximately 8.3 seconds
  printf("Watchdog active\n");

  //   sendEmailAlert("Grow%20box%20(re)started");
  printf("Setup ready, starting loops:\n");
  // run_ntp_test();

  struct repeating_timer Timer1sec;
  add_repeating_timer_ms(-1000, run1sec, NULL, &Timer1sec); // Calls runRepeatedly every second
  struct repeating_timer Timer5sec;
  add_repeating_timer_ms(-5000, run5sec, NULL, &Timer5sec); // Calls runRepeatedly every second
  struct repeating_timer Timer1min;
  add_repeating_timer_ms(-60000, run1min, NULL, &Timer1min); // Calls runRepeatedly every minute
  struct repeating_timer Timer30min;
  add_repeating_timer_ms(-60000, run30min, NULL, &Timer30min); // Calls runRepeatedly every minute

  absolute_time_t NextRefresh = make_timeout_time_ms(5000); // 5sec from now
  while (1)
  {
    sleep_until(NextRefresh);                 // The microcontroller is in sleep mode, only wakes up when a repeating_timer triggers.
    watchdog_update();                        // Pet watchdog
    NextRefresh = make_timeout_time_ms(5000); // 5sec from now
  }
  cyw43_arch_deinit();
  return 0;
}

/*
void getWirelessStatus()
{
  if (Debug)
  {
    //printf("Wireless report:\n");
    cyw43_wifi_link_status();
    printf("\n");
  }
}
*/