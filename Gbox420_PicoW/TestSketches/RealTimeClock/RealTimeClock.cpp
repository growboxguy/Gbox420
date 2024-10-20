/*! \file
 *  \brief     Test setting the integrated Real Time Clock
 *  \details   
 *  \attention cd build
 *             cmake -DPICO_BOARD=pico_w .. -G "MinGW Makefiles"
 *             mingw32-make.exe
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/raspberry-pi-pico-w
 *  \version   4.20
 */

#include <stdio.h>
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"

// Global variable initialization
datetime_t CurrentTime;
char CurrentTime_buffer[128];

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
    datetime_to_str(CurrentTime_buffer, sizeof(CurrentTime_buffer), &CurrentTime);
    printf("%s\n", CurrentTime_buffer);
}

int main()
{
    stdio_init_all();
    printf("Starting 15 sec delay...\n");
    sleep_ms(15000);
    
    initializeRTC();
   
    absolute_time_t LastRefresh = get_absolute_time();
    while (true)
    {
        if (absolute_time_diff_us(LastRefresh, get_absolute_time()) > 5000000) // 5sec
        {
            getRTC();
            LastRefresh = get_absolute_time();
        }
        sleep_ms(1000);  //1sec
    }
    return 0;
}