/*! \file
 *  \brief     Wifi connection test
 *  \details   Requires 2 parameters -DWIFI_SSID="WifiNetworkPasswordName" -DWIFI_PASSWORD="WifiNetworkPassword"
 *  \attention cd build
 *             cmake -DPICO_BOARD=pico_w -DWIFI_SSID="WifiNetworkPasswordName" -DWIFI_PASSWORD="WifiNetworkPassword" .. -G "MinGW Makefiles"
 *             mingw32-make.exe
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/raspberry-pi-pico-w
 *  \version   4.20
 */

#include <stdio.h>
#include "hardware/rtc.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/util/datetime.h"
// #include <string.h>

// Global constants
#define MaxShotTextLength 128 ///< Default char * buffer length for storing mutiple words. Memory intense!

// Global variable initialization
datetime_t CurrentTime;
char ShortMessage[MaxShotTextLength]; ///< Temp storage for assembling short text messages (Log entries, Error messages,etc)

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
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000))
        {
            printf("failed\n");
        }
        else
            printf("...done\n");
    }
}

// Update built-in Real Time Clock with an NTP query
void updateRTC()
{
    datetime_t timeTemp = {
        .year = 2023,
        .month = 04,
        .day = 20,
        .dotw = 4, // 0 is Sunday, so 4 is Thursday
        .hour = 16,
        .min = 20,
        .sec = 00};
    rtc_set_datetime(&timeTemp);
    printf("done\n");
}

// Initialize the built-in RTC
void initializeRTC()
{
    printf("Initializing RTC...");
    rtc_init();
    updateRTC();
}

// Query current time from local RTC
void getRTC()
{
    rtc_get_datetime(&CurrentTime);
    datetime_to_str(ShortMessage, MaxShotTextLength, &CurrentTime);
    printf("%s", ShortMessage);
}

int main()
{
    stdio_init_all();

    printf("Starting 15 sec delay...");
    sleep_ms(15000);
    if (watchdog_caused_reboot()) {
        printf("Rebooted by Watchdog!\n");
    } else {
        printf("Clean boot\n");
    }  
    
    initializeWiFi();
    initializeRTC();
    watchdog_enable(0x7fffff, 1);  // Maximum of 0x7fffff, which is approximately 8.3 seconds

    // run_ntp_test();
    // cyw43_arch_deinit();
    int LoopCount = 0;
    absolute_time_t LastRefresh = get_absolute_time();
    while (LoopCount < 12)
    {
#if PICO_CYW43_ARCH_POLL
        cyw43_arch_poll(); // check for WiFi driver or lwIP work that needs to be done
        sleep_ms(1);
#endif
        if (absolute_time_diff_us(LastRefresh, get_absolute_time()) > 5000000) // 5sec
        {
            watchdog_update();
            getRTC();
            printf(" Listening...\n");
            LastRefresh = get_absolute_time();
            LoopCount++;
        }
    }
    cyw43_arch_deinit();
    return 0;
}