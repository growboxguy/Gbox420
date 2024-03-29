#include "RealTimeClock-NTPWithLWIP.h"

int main()
{
    stdio_init_all();
    printf("\nStarting 10 sec delay...\n");
    sleep_ms(10000);
    printf("\nRTC-NTP test starting\n");
    printf("Creating tasks\n");
    xTaskCreate(wifiTask, "WiFi checker", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L3, NULL);   ///< Connect-ReConnect to WiFi
    xTaskCreate(rtcTask, "RTC update", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L2, NULL);      ///< Update Real Time Clock using NTP
    xTaskCreate(heartbeatTask, "Heartbeat", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L1, NULL); ///< Blink built-in LED
    printf("Starting scheduler\n");
    vTaskStartScheduler();

    cyw43_arch_deinit();
    return 0;
}

void wifiTask(void *pvParameters)
{
    int WifiInitResult = cyw43_arch_init();
    if (WifiInitResult != 0)
    {
        printf("WiFi init failed: %d\n", WifiInitResult);
    }
    else
    {
        printf("WiFi initialized\n");
        connectWiFi();
        while (1)
        {
            vTaskDelay(30000 / portTICK_PERIOD_MS);                                            // Delay for 30sec
            printf("WiFi status: %d\n", cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA)); // Returns the status of the WiFi link: CYW43_LINK_DOWN(0)-link is down,CYW43_LINK_JOIN(1)-Connected to WiFi,CYW43_LINK_NOIP(2)-Connected to WiFi, but no IP address,CYW43_LINK_UP  (3)-Connect to WiFi with an IP address,CYW43_LINK_FAIL(-1)-Connection failed,CYW43_LINK_NONET(-2)-No matching SSID found (could be out of range, or down),CYW43_LINK_BADAUTH(-3)-Authentication failure
            if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP)
            {
                connectWiFi();
            }
        }
    }
}

// Initialize WiFi and Connect to local network
void connectWiFi()
{
    cyw43_arch_enable_sta_mode();                                                                                         // Enables Wi-Fi STA (Station) mode
    int WifiConnectResult = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000); // Max 10sec
    if (WifiConnectResult != 0)
    {
        printf("Connecting to %s failed: %d\n", WIFI_SSID, WifiConnectResult);
    }
    else
    {
        char *IPAddress = ipaddr_ntoa(netif_ip4_addr(&cyw43_state.netif[0]));
        printf("Connected to %s - %s\n", WIFI_SSID, IPAddress);
    }
}

void rtcTask(void *pvParameters)
{
    vTaskDelay(10000 / portTICK_PERIOD_MS); // Delay for 10sec to give time to WiFi to connect
    printf("Initializing RTC...\n");
    ip_addr_t NTPServerIP;
    DnsLookup(NTPServer1.NtpServerDNS, &NTPServerIP);
    updateRTC(1713630000, 0); // Fake receiving an NTP response
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setserver(0, &NTPServerIP);
    sntp_init();
    while (1)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS); // Delay for 10sec
        getRTC();
    }
}

void updateRTC(u32_t sec, u32_t frac)
{
    printf("NTP callback received\n");
    time_t timestamp = sec;
    struct tm *time = gmtime(&timestamp);

    datetime_t datetime = {
        .year = (int16_t)(time->tm_year + 1900),
        .month = (int8_t)(time->tm_mon + 1),
        .day = (int8_t)time->tm_mday,
        .dotw = (int8_t)time->tm_wday,
        .hour = (int8_t)time->tm_hour,
        .min = (int8_t)time->tm_min,
        .sec = (int8_t)time->tm_sec};
    datetime_to_str(CurrentTime_buffer, sizeof(CurrentTime_buffer), &datetime);
    printf("Updating RTC to: %s\n", CurrentTime_buffer);
    if (rtc_set_datetime(&datetime))
    {
        printf("done\n");
    }
    else
    {
        printf("failed");
    }
}

// Query current time from local RTC
void getRTC()
{
    rtc_get_datetime(&CurrentTime);
    datetime_to_str(CurrentTime_buffer, sizeof(CurrentTime_buffer), &CurrentTime);
    printf("%s\n", CurrentTime_buffer);
}

///< Blink built-in LED on Pico W - Requires WiFi initialization cyw43_arch_init() , since the LED is connected to the WiFi controller
void heartbeatTask(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(1000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, ledStatus);
        ledStatus = !ledStatus;
    }
}

///< DNS client
bool DnsLookup(char *DnsName, ip_addr_t *ResultIP)
{
    dnsLookupSuccess = false;
    dnsLookupInProgress = true;
    printf("   Looking up IP for %s...", DnsName);
    err_t err = dns_gethostbyname(DnsName, ResultIP, DnsLookupResult, ResultIP);
    if (err == ERR_OK) // DNS name found in cache and loaded into ResultIP
    {
        printf("Found cached address\n");
        return true;
    }
    absolute_time_t Timeout = make_timeout_time_ms(15000); // 15sec from now
    while (dnsLookupInProgress)                            // Waiting for the DNS lookup to finish and DnsLookupResult callback to trigger
    {
        if (get_absolute_time() > Timeout)
        {
            printf("DNS lookup timeout\n");
            return false;
        }
        vTaskDelay(500);
    }
    return dnsLookupSuccess;
}

/// @brief Callback when DNS lookup is finished
/// @param Hostname The DNS name
/// @param FoundIP The IP for the Name
/// @param ResultIP
void DnsLookupResult(const char *Hostname, const ip_addr_t *FoundIP, void *ResultIP) // DNS lookup callback
{
    if (FoundIP)
    {
        printf("Found address: %s\n", ipaddr_ntoa(FoundIP));
        ip_addr_copy(*(ip_addr_t *)ResultIP, *FoundIP);
        dnsLookupSuccess = true;
    }
    else
    {
        printf("DNS lookup failed\n");
    }
    dnsLookupInProgress = false;
}