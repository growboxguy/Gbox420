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
  printf("\nGbox420 FreeRTOS initializing\n");
  GboxSettings = loadSettings(false);
  Debug = &GboxSettings->Debug;
  Metric = &GboxSettings->Metric;
  rtcInit(); ///< Initialize Real Time Clock and set a pre-defined starting date
  printf("Creating tasks...");
  xTaskCreate(watchdogTask, "Watchdog", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L3, NULL);                 ///< Watchdog for crash detection and automatic restart
  xTaskCreate(connectivityTask, "Connectivity checker", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L2, NULL); ///< Connect-ReConnect to WiFi, Sync the Real Time Clock using NTP, Make sure MQTT server is connected
                                                                                                                 // xTaskCreate(hempyTask, "Hempy checker", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L1, NULL);               ///< Hempy module
  printf("done\n");
  vTaskStartScheduler();
  cyw43_arch_deinit(); ///< Processing will not get here unless a task calls vTaskEndScheduler()
  return 0;
}

// Hempy module
void hempyTask(void *pvParameters)
{
  HempyModule1 = new HempyModule(&GboxSettings->HempyModule1, GboxSettings);
  xTimerStart(xTimerCreate("1Sec", pdMS_TO_TICKS(1000), pdTRUE, (void *)1, run1Sec), 1);      ///< Create 1sec software timer
  xTimerStart(xTimerCreate("5Sec", pdMS_TO_TICKS(5000), pdTRUE, (void *)2, run5Sec), 1);      ///< Create 5sec software timer
  xTimerStart(xTimerCreate("1Min", pdMS_TO_TICKS(60000), pdTRUE, (void *)3, run1Min), 1);     ///< Create 1min software timer
  xTimerStart(xTimerCreate("30Min", pdMS_TO_TICKS(1800000), pdTRUE, (void *)4, run30Min), 1); ///< Create 30min software timer

  // Task should not exit
  while (1)
  {
    vTaskDelay(portMAX_DELAY);
  }
}

// Runs every 1 sec
void run1Sec(TimerHandle_t xTimer)
{
  HempyModule1->run1sec();
}
///< Runs every 5 sec
void run5Sec(TimerHandle_t xTimer)
{
  HempyModule1->run5sec();
}
///< Runs every 1 min
void run1Min(TimerHandle_t xTimer)
{
  HempyModule1->run1min();
}
///< Runs every 30 min
void run30Min(TimerHandle_t xTimer)
{
  HempyModule1->run30min();
}

// Monitor program execution, If the program fails to reset the timer periodically, it indicates a fault, triggering a system reset
void watchdogTask(void *pvParameters)
{
  printf("Initializing Watchdog...");
  watchdog_enable(0x7fffff, 1); // Maximum of 0x7fffff, which is approximately 8.3 seconds
  printf("done\n");
  while (1)
  {
    vTaskDelay(pdMS_TO_TICKS(7000)); // Delay 7sec
    watchdog_update();               // Pet watchdog
  }
}

void connectivityTask(void *pvParameters)
{
  printf("Initializing WiFi\n");
  int WifiInitResult = cyw43_arch_init();
  while (WifiInitResult != 0)
  {
    printf("WiFi init failed: %d\n", WifiInitResult);
    vTaskDelay(pdMS_TO_TICKS(WIFI_TIMEOUT * 1000));
    WifiInitResult = cyw43_arch_init();
  }
  NtpPcb = udp_new_ip_type(IPADDR_TYPE_ANY); // NTP: Creates a new UDP Protocol Control Block (PCB) with both IPv4 and IPv6 support
  udp_recv(NtpPcb, ntpReceived, NULL);       // NTP: Register the callback function to handle incoming UDP packets received on the UDP PCB
  connectWiFi();
  MqttClientDefault = new MqttClient(&GboxSettings->HempyMqttServer1, mqttDataReceived);
  uint8_t ConnectivityCounter = WIFI_TIMEOUT; // Count the seconds since the last WiFi connectivity check
  while (1)
  {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, ledStatus); // Blink Internal LED
    ledStatus = !ledStatus;
    if (++ConnectivityCounter >= WIFI_TIMEOUT)
    {
      ConnectivityCounter = 0;
      rtcGetCurrentTime(true);
      printf("WiFi status: %s\n", toText_WiFiStatus(cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA))); // Returns the status of the WiFi link: CYW43_LINK_DOWN(0)-link is down,CYW43_LINK_JOIN(1)-Connected to WiFi,CYW43_LINK_NOIP(2)-Connected to WiFi, but no IP address,CYW43_LINK_UP  (3)-Connect to WiFi with an IP address,CYW43_LINK_FAIL(-1)-Connection failed,CYW43_LINK_NONET(-2)-No matching SSID found (could be out of range, or down),CYW43_LINK_BADAUTH(-3)-Authentication failure
      if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP)
      {
        connectWiFi();
      }
      else // WiFi is up and has an IP
      {
        if (!NtpSynced) // if NTP is not synced
        {
          ntpRequest();
        }
        else
        {
          NtpSynced++; // NtpSynced is uint8_t, overflows after 255 checks -> Forces an NTP update every hour with WIFI_TIMER set to 15sec
        }

        printf("MQTT status: %s\n", MqttClientDefault->mqttIsConnectedText(true)); // Returns the status of the WiFi link: CYW43_LINK_DOWN(0)-link is down,CYW43_LINK_JOIN(1)-Connected to WiFi,CYW43_LINK_NOIP(2)-Connected to WiFi, but no IP address,CYW43_LINK_UP  (3)-Connect to WiFi with an IP address,CYW43_LINK_FAIL(-1)-Connection failed,CYW43_LINK_NONET(-2)-No matching SSID found (could be out of range, or down),CYW43_LINK_BADAUTH(-3)-Authentication failure
        if (!MqttClientDefault->mqttIsConnected())
        {
          MqttClientDefault->mqttConnectTrigger();
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000)); // Wait one sec
  }
}

// Initialize WiFi and Connect to local network
bool connectWiFi()
{
  cyw43_arch_lwip_begin();
  cyw43_arch_enable_sta_mode();                                                                                                       // Enables Wi-Fi STA (Station) mode
  int WifiConnectResult = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, WIFI_TIMEOUT * 1000); // Try connecting to WiFi. If the timeout elapses, the attempt may still succeed afterward.
  cyw43_arch_lwip_end();
  if (WifiConnectResult != 0)  //OK:0,TIMEOUT:-1,GENERIC:-2,NO_DATA:-3,NOT_PERMITTED:-4,INVALID_ARG:-5,IO:-6,BADAUTH:-7,CONNECT_FAILED:-8,INSUFFICIENT_RESOURCES:-9
  {
    printf("Connecting to %s failed: %d\n", WIFI_SSID, WifiConnectResult);
    return false;
  }
  else
  {
    char *IPAddress = ipaddr_ntoa(netif_ip4_addr(&cyw43_state.netif[0]));
    printf("Connected to %s - %s\n", WIFI_SSID, IPAddress);
    ntpRequest();
    return true;
  }
}

///< MQTT callback when a data arrives on a Subscribed topic
void mqttDataReceived(char *TopicReceived, char *DataReceived)
{
  // printf("MQTT topic: %s\nMQTT data: %s\n", TopicReceived, DataReceived);
  HempyModule1->mqttDataReceived(TopicReceived, DataReceived);
}

///< Real Time Clock (RTC) section
#pragma region RTC
// Initialize Real Time Clock and set a pre-defined 'starting' date
void rtcInit()
{
  printf("Initializing RTC...");
  rtc_init(); // Initialize "hardware/rtc.h"
  datetime_t timeTemp = {
      .year = 2024, // tm_year: years since 1900
      .month = 4,   // tm_mon: months since January (0-11)
      .day = 20,
      .dotw = 5, // tm_wday: days since Sunday (0-6)
      .hour = 16,
      .min = 20,
      .sec = 00};
  if (rtc_set_datetime(&timeTemp))
    printf("done\n");
  else
    printf("failed\n");
}

// Current time from local RTC
char *rtcGetCurrentTime(bool PrintToSerial)
{
  datetime_t CurrentDateTime;
  rtc_get_datetime(&CurrentDateTime);
  datetime_to_str(ShortMessage, MaxShotTextLength, &CurrentDateTime);
  if (PrintToSerial)
  {
    printf("%s\n", ShortMessage);
  }
  return ShortMessage;
}
#pragma endregion

///< Network Time Protocol (NTP) section
#pragma region NTP
// Make an NTP request
void ntpRequest()
{
  NtpSynced = 0;
  if (GboxSettings->NtpServer1.NtpServerIP[0] != '\0')
  {
    ip4addr_aton(GboxSettings->NtpServer1.NtpServerIP, &NtpServerIP); // If NtpServerDNS is defined and the DNS lookup is successful this will be overwritten
  }
  if (GboxSettings->NtpServer1.NtpServerDNS[0] != '\0')
  {
    dnsLookup(GboxSettings->NtpServer1.NtpServerDNS, &NtpServerIP);
  }
  cyw43_arch_lwip_begin();
  struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, 48, PBUF_RAM); // 48 is the NTP message length
  uint8_t *req = (uint8_t *)p->payload;
  memset(req, 0, 48);
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
  if (ip_addr_cmp(addr, &NtpServerIP) && port == GboxSettings->NtpServer1.NtpServerPort && p->tot_len == 48 && mode == 0x4 && stratum != 0)
  {
    uint8_t seconds_buf[4] = {0};
    pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
    uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
    uint32_t seconds_since_1970 = seconds_since_1900 - 2208988800; //  2208988800 seconds between 1 Jan 1900 and 1 Jan 1970
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
    datetime_to_str(CurrentTimeText, sizeof(CurrentTimeText), &timeTemp);
    if (rtc_set_datetime(&timeTemp))
    {
      printf("RTC synced: %s\n", CurrentTimeText);
      NtpSynced = 1;
    }
    else
      printf("RTC update failed\n");
  }
  else
  {
    printf("Invalid NTP response\n");
  }
  pbuf_free(p);
}
#pragma endregion

///< DNS client section
#pragma region DNS
/// @brief Lookup a DNS name while blocking processing until a result is received. Cannot be called from a timer interrupt like run1sec,run5sec,run1min,report
/// @param DnsName Pointer to char containing the name to lookup
/// @param ResultIP Pointer to the ip_addr_t object where the lookup result will be stored
/// @return true: Lookup successful, false: lookup failed
bool dnsLookup(char *DnsName, ip_addr_t *ResultIP)
{
  dnsLookupSuccess = false;
  dnsLookupInProgress = true;
  printf("Looking up IP for %s\n", DnsName);
  cyw43_arch_lwip_begin();
  err_t err = dns_gethostbyname(DnsName, ResultIP, dnsLookupResult, ResultIP);
  cyw43_arch_lwip_end();
  if (err == ERR_OK) // DNS name found in cache and loaded into ResultIP
  {
    printf("Found cached address: %s - %s\n", ipaddr_ntoa(ResultIP), DnsName);
    return true;
  }
  absolute_time_t Timeout = make_timeout_time_ms(DNS_TIMEOUT * 1000);
  while (dnsLookupInProgress)                            // Waiting for the DNS lookup to finish and dnsLookupResult callback to trigger
  {
    if (absolute_time_diff_us(Timeout, get_absolute_time()) > 0)
    {
      printf("DNS lookup timeout - %s\n", DnsName);
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
void dnsLookupResult(const char *Hostname, const ip_addr_t *FoundIP, void *ResultIP) // DNS lookup callback
{
  if (FoundIP)
  {
    printf("Found address: %s for %s\n", ipaddr_ntoa(FoundIP), Hostname);
    ip_addr_copy(*(ip_addr_t *)ResultIP, *FoundIP);
    dnsLookupSuccess = true;
  }
  else
  {
    printf("DNS lookup failed for %s\n", Hostname);
  }
  dnsLookupInProgress = false;
}
#pragma endregion