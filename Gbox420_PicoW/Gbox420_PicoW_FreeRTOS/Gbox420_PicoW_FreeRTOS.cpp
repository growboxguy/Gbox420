#include "Gbox420_PicoW_FreeRTOS.h"

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
  GboxSettings = loadSettings(false);
  Debug = &GboxSettings->Debug;
  Metric = &GboxSettings->Metric;
  printf("Creating tasks\n");
  xTaskCreate(watchdogTask, "Watchdog", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L3, NULL);   ///< Blink built-in LED
  xTaskCreate(wifiTask, "WiFi checker", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L2, NULL);   ///< Connect-ReConnect to WiFi
  xTaskCreate(ntpTask, "NTP update", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L2, NULL);      ///< Update Real Time Clock using NTP
  xTaskCreate(heartbeatTask, "Heartbeat", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L1, NULL); ///< Blink built-in LED
  printf("Starting scheduler\n");
  vTaskStartScheduler();

  cyw43_arch_deinit();
  return 0;
}

void watchdogTask(void *pvParameters)
{
  printf("Initializing Watchdog...");
  watchdog_enable(0x7fffff, 1); // Maximum of 0x7fffff, which is approximately 8.3 seconds
  printf("done\n");
  while (1)
  {
    vTaskDelay(7000 / portTICK_PERIOD_MS); // Delay for 7sec
    watchdog_update();                     // Pet watchdog
  }
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

void ntpTask(void *pvParameters)
{
  vTaskDelay(GboxSettings->NtpServer1.StartupDelaySeconds * 1000 / portTICK_PERIOD_MS); // Delay to allow time for WiFi connection to establish
  printf("Initializing RTC...\n");
  DnsLookup(GboxSettings->NtpServer1.NtpServerDNS, &NtpServerIP);
  NtpPcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  udp_recv(NtpPcb, ntpReceived, NULL);
  rtc_init(); // Initialize "hardware/rtc.h"
  ntpRequest();
  while (1)
  {
    vTaskDelay(10000 / portTICK_PERIOD_MS); // Delay for 10sec
    getCurrentTime(true);
  }
}

// Make an NTP request
void ntpRequest()
{
  cyw43_arch_lwip_begin();
  struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
  uint8_t *req = (uint8_t *)p->payload;
  memset(req, 0, NTP_MSG_LEN);
  req[0] = 0x1b;
  udp_sendto(NtpPcb, p, &NtpServerIP, GboxSettings->NtpServer1.NtpServerPort);
  pbuf_free(p);
  cyw43_arch_lwip_end();
}

// NTP data received
void ntpReceived(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
  uint8_t mode = pbuf_get_at(p, 0) & 0x7;
  uint8_t stratum = pbuf_get_at(p, 1);
  datetime_t timeTemp;
  // Check the result
  if (ip_addr_cmp(addr, &NtpServerIP) && port == GboxSettings->NtpServer1.NtpServerPort && p->tot_len == NTP_MSG_LEN && mode == 0x4 && stratum != 0)
  {
    uint8_t seconds_buf[4] = {0};
    pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
    uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
    uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
    time_t epoch = seconds_since_1970;
    struct tm *utc = gmtime(&epoch); // https://cplusplus.com/reference/ctime/tm/
    timeTemp = {
        .year = (int16_t)(utc->tm_year + 1900), // utc->tm_year: years since 1900
        .month = (int8_t)(utc->tm_mon + 1),     // utc->tm_mon: months since January (0-11)
        .day = (int8_t)utc->tm_mday,
        .dotw = (int8_t)utc->tm_wday, // utc->tm_wday: days since Sunday (0-6)
        .hour = (int8_t)(utc->tm_hour + GboxSettings->NtpServer1.TimeZoneDifference),
        .min = (int8_t)utc->tm_min,
        .sec = (int8_t)utc->tm_sec};
  }
  else
  {
    timeTemp = {
        .year = 2024, // tm_year: years since 1900
        .month = 4,   // tm_mon: months since January (0-11)
        .day = 20,
        .dotw = 5, // tm_wday: days since Sunday (0-6)
        .hour = 16,
        .min = 20,
        .sec = 00};
    printf("Invalid NTP response, fallback set\n");
  }
  datetime_to_str(CurrentTimeText, sizeof(CurrentTimeText), &timeTemp);
  printf("Setting RTC to: %s ", CurrentTimeText);
  if (rtc_set_datetime(&timeTemp))
    printf("done\n");
  else
    printf("failed\n");
  pbuf_free(p);
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