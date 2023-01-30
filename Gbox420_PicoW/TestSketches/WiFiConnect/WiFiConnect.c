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
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
// #include <string.h>

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

int main()
{
    stdio_init_all();

    printf("Starting 15 sec delay...");
    sleep_ms(15000);

    initializeWiFi();

    absolute_time_t LastRefresh = get_absolute_time();
    while (true)
    {
#if PICO_CYW43_ARCH_POLL
        cyw43_arch_poll(); // check for WiFi driver or lwIP work that needs to be done
        sleep_ms(1);
#endif
        if (absolute_time_diff_us(LastRefresh, get_absolute_time()) > 5000000) // 5sec
        {
            printf("Listening...\n");
            LastRefresh = get_absolute_time();
        }
    }
    cyw43_arch_deinit();
    return 0;
}