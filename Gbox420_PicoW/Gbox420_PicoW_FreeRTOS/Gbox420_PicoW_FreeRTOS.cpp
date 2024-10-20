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
  GboxSettings = loadSettings(false); ///< Load the default settings from Settings.h
  Debug = &GboxSettings->Debug;       ///< Set global variable
  if (Debug)
  {
    printf("Debug ENABLED, last DebugMessage: %s\n", DebugMessage); ///< If debug is enabled, print the last stored debug message
  }
  strcpy(DebugMessage, "Gbox420 running");
  Metric = &GboxSettings->Metric;                                   ///< Set global variable
  MqttPublishSemaphore = xSemaphoreCreateBinary();                  ///< Initialize a semaphore used during network operations
  xSemaphoreGive(MqttPublishSemaphore);                             ///< Make sure the semaphore is available
  rtcInit();                                                        ///< Initialize Real Time Clock and set a pre-defined starting date
  GboxModule1 = new GboxModule(&GboxSettings->Gbox1, GboxSettings); ///< Stripped down core module only containing a Sound component
  timer_hw->dbgpause = 0;                                           // Do not pause HW timer when debug is active
  printf("Initializing Watchdog...");                               ///< Watchdog should be the last to initialize
  watchdog_enable(0x7fffff, true);                                  ///< Maximum of 0x7fffff, which is approximately 8.3 seconds. true: Pause on debug
  printf("done\n");
  printf("Starting timers...");
  xTimerStart(xTimerCreate("1Sec", pdMS_TO_TICKS(1000), pdTRUE, (void *)1, run1Sec), 1);      ///< Create 1sec software timer
  xTimerStart(xTimerCreate("5Sec", pdMS_TO_TICKS(5000), pdTRUE, (void *)2, run5Sec), 1);      ///< Create 5sec software timer
  xTimerStart(xTimerCreate("1Min", pdMS_TO_TICKS(60000), pdTRUE, (void *)3, run1Min), 1);     ///< Create 1min software timer
  xTimerStart(xTimerCreate("30Min", pdMS_TO_TICKS(1800000), pdTRUE, (void *)4, run30Min), 1); ///< Create 30min software timer
  printf("done\n");
  printf("Creating tasks...");
  xTaskCreate(connectivityTask, "Connectivity checker", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L2, NULL); ///< Connect-ReConnect to WiFi, Sync the Real Time Clock using NTP, Make sure MQTT server is connected
  xTaskCreate(serialReadTask, "Serial checker", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L2, NULL);         ///< Waits for incoming commands from stdin (Serial port)
  // xTaskCreate(hempyTask, "Hempy checker", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L1, NULL);               ///< Hempy module
  printf("done\n");
  vTaskStartScheduler();
  cyw43_arch_deinit(); ///< Processing will not get here unless a task calls vTaskEndScheduler()
  return 0;
}

/*
void vTaskDelete( TaskHandle_t xTask )
{
printf("NICE TRY\n");
}
*/

// Runs every 1 sec
void run1Sec(TimerHandle_t xTimer)
{
  if (*Debug)
    printf("1sec\n");
  watchdog_update(); // Pet watchdog
  GboxModule1->run1sec();
  // HempyModule1->run1sec();
}

///< Runs every 5 sec
void run5Sec(TimerHandle_t xTimer)
{
  if (*Debug)
    printf("5sec\n");
  watchdog_update(); // Pet watchdog
  GboxModule1->run5sec();
  // HempyModule1->run5sec();
  // mqttPublish(NULL, "{\"Gbox420\":{\"Debug\":1,\"Metric\":1\"}}");               // Publish to the default topic from Settings.h (PubTopic)
  // mqttPublish("NotDefaultTopic/", "{\"Gbox420\":{\"Debug\":0,\"Metric\":0\"}}"); // Publish to a specified topic
}

///< Runs every 1 min
void run1Min(TimerHandle_t xTimer)
{
  if (*Debug)
    printf("1min\n");
  watchdog_update(); // Pet watchdog
  GboxModule1->run1min();
  // HempyModule1->run1min();
}

///< Runs every 30 min
void run30Min(TimerHandle_t xTimer)
{
  if (*Debug)
    printf("30min\n");
  watchdog_update(); // Pet watchdog
  GboxModule1->run30min();
  // HempyModule1->run30min();
}

/// @brief Process incoming command sent from the Serial Monitor Expected text command format: NAME_COMMAND , where NAME is the objects's name from Settings.h example: Sound1_EE   Line ending: LF
void serialReadTask(void *pvParameters)
{
  char Message[MaxShotTextLength];
  while (1)
  {
    fgets(Message, MaxShotTextLength, stdin); // Blocking code! Waits until Serial input is received. Only triggers if the incoming data ends with LF (Line feed \n), make sure the Serial monitor's line ending is set to LF
    Message[strcspn(Message, "\n")] = '\0';   // Remove the newline character from the end of Message
    printf("Serial input: %s\n", Message);
    mqttDataReceived(Message, NULL); // Fake an incoming MQTT command. TODO: Add support for passing data along the command
  }
}

// Initialize everything network related, then periodically check their status (reconnect if needed). After the initialization is done the internal LED should start blinking every second
void connectivityTask(void *pvParameters)
{
  printf("Initializing WiFi\n");
  int WiFiInitResult = cyw43_arch_init();
  while (WiFiInitResult != 0)
  {
    printf("WiFi init failed: %d\n", WiFiInitResult);
    vTaskDelay(pdMS_TO_TICKS(WIFI_TIMEOUT * 1000));
    WiFiInitResult = cyw43_arch_init();
  }
  NtpPcb = udp_new_ip_type(IPADDR_TYPE_ANY); // NTP: Creates a new UDP Protocol Control Block (PCB) with both IPv4 and IPv6 support
  udp_recv(NtpPcb, ntpReceived, NULL);       // NTP: Register the callback function to handle incoming UDP packets received on the UDP PCB
  connectWiFi();
  DefaultMqttClient = new MqttClient(&GboxSettings->MqttServer1, mqttDataReceived); ///< Create an MQTT client object settings from Settings.h , and set a callback function when data is received from a subscribed topic
  while (1)
  {
    if (++ConnectivityCounter >= WIFI_TIMEOUT) // Is it time to do a new WiFi check?
    {
      ConnectivityCounter = 0;
      rtcGetCurrentTime(true);
      cyw43_arch_lwip_begin();
      int CurrentWiFiStatus = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
      cyw43_arch_lwip_end();
      printf("WiFi status: %s\n", toText_WiFiStatus(CurrentWiFiStatus)); // Returns the status of the WiFi link: CYW43_LINK_DOWN(0)-link is down,CYW43_LINK_JOIN(1)-Connected to WiFi,CYW43_LINK_NOIP(2)-Connected to WiFi, but no IP address,CYW43_LINK_UP  (3)-Connect to WiFi with an IP address,CYW43_LINK_FAIL(-1)-Connection failed,CYW43_LINK_NONET(-2)-No matching SSID found (could be out of range, or down),CYW43_LINK_BADAUTH(-3)-Authentication failure
      if (CurrentWiFiStatus != CYW43_LINK_UP)
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
          NtpSynced++; // NtpSynced is uint8_t, overflows after 255 checks -> Forces an NTP update every hour with WIFI_TIMER set to 15sec. If NtpSynced is changed to uint16_t the sync delay is ~11 days
        }
        printf("MQTT status: %s\n", DefaultMqttClient->mqttIsConnectedText(true)); // Returns the status of the WiFi link: CYW43_LINK_DOWN(0)-link is down,CYW43_LINK_JOIN(1)-Connected to WiFi,CYW43_LINK_NOIP(2)-Connected to WiFi, but no IP address,CYW43_LINK_UP  (3)-Connect to WiFi with an IP address,CYW43_LINK_FAIL(-1)-Connection failed,CYW43_LINK_NONET(-2)-No matching SSID found (could be out of range, or down),CYW43_LINK_BADAUTH(-3)-Authentication failure
        if (!DefaultMqttClient->mqttIsConnected())                                 // If MQTT server is not connected -> Trigger a connection attempt
        {
          DefaultMqttClient->mqttConnectTrigger();
        }
      }
    }
    heartbeat(); // Blinks the onboard LED, and delays the next While loop by 1sec
  }
}

// Controls the onboard LED, blink every sec: MQTT connected, blink every 0.25sec: MQTT disconnected. !! Makes the caller task delay a total of 1 sec !!
void heartbeat()
{
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, ledStatus); // Blink Internal LED
  ledStatus = !ledStatus;                                // Blink Internal LED
  if (DefaultMqttClient->mqttIsConnected())
  {
    vTaskDelay(pdMS_TO_TICKS(1000)); // Wait one sec
  }
  else // If MQTT server is not connected: Blink the LED rapidly (every 0.25 sec)
  {
    for (uint8_t i = 0; i < 4; i++) // Run 3 times, with a total of ~750ms
    {
      vTaskDelay(pdMS_TO_TICKS(250));                        // Wait 0.25 sec
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, ledStatus); // Blink Internal LED
      ledStatus = !ledStatus;                                // Invert status for the next loop
    }
    vTaskDelay(pdMS_TO_TICKS(250)); // Wait 0.25 sec
  }
}

// Initialize WiFi and Connect to local network
bool connectWiFi()
{
  cyw43_arch_enable_sta_mode();                                                                                                       // Enables Wi-Fi STA (Station) mode
  ledStatus = true;                                                                                                                   // Turn on onboard LED to indicate the start of the WiFi connection attempt
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, ledStatus);                                                                              // Onboard LED constantly on: WiFi connection in progress.
  int WiFiConnectResult = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, WIFI_TIMEOUT * 1000); // Try connecting to WiFi. If the timeout elapses, the attempt may still succeed afterward.
  if (WiFiConnectResult != 0)
  {
    printf("Connecting to %s failed: %d %s\n", WIFI_SSID, WiFiConnectResult, toText_WiFiConnectResult(WiFiConnectResult));
    return false;
  }
  else
  {
    char *IPAddress = ipaddr_ntoa(netif_ip4_addr(&cyw43_state.netif[0]));
    printf("Connected to %s - %s\n", WIFI_SSID, IPAddress);
    ConnectivityCounter = WIFI_TIMEOUT; // Force a connectivity check
    return true;
  }
}

/**
 * @brief MQTT callback when a data arrives on a Subscribed topic
 *
 * @param SubTopicReceived The subtopic of the arrived command (The known part of the subscribed topic is already stripped at this point)
 * @param DataReceived Additional data (if any) passed along the command
 */
void mqttDataReceived(char *SubTopicReceived, char *DataReceived)
{
  // printf("MQTT topic: %s\nMQTT data: %s\n", TopicReceived, DataReceived);
  if (isThisForMe(GboxSettings->Name, SubTopicReceived)) //< If the topic starts with Gbox_ -> process the command
  {
    if (strcmp(ShortMessage, "D") == 0)
    {
      *Debug = !*Debug;
      printf("Debug %s\n", toText_enabledDisabled(*Debug));
    }
    /*
    else if (strcmp(ShortMessage, "Ee") == 0)
    {
     playEE();
    }
    */
  }
  else // Pass along the command
  {
    GboxModule1->mqttDataReceived(SubTopicReceived, DataReceived);
    // HempyModule1->mqttDataReceived(SubTopicReceived, DataReceived);  //TODO: mqttDataReceived should return true if the command was processed within the module
  }
}

/**
 * @brief Publish an MQTT message to a Topic containing a JSON formatted Data
 * Example:
 * MQTT reporting: Gbox420/{"Log":{"Sound1":{"S":"1"},"Gbox1":{"M":"1","D":"1"}}}
 */
void mqttPublish(const char *Topic, const char *Data)
{
  if (DefaultMqttClient != nullptr)
  {
    DefaultMqttClient->mqttPublish(Topic, Data);
  }
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
  char CurrentTimeText[MaxShotTextLength]; ///< Store current time in text format
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
  if (err == ERR_OK) // DNS name found in cache and loaded into ResultIP
  {
    printf("Found cached address %s - %s\n", ipaddr_ntoa(ResultIP), DnsName);
    return true;
  }
  cyw43_arch_lwip_end();
  absolute_time_t Timeout = make_timeout_time_ms(DNS_TIMEOUT * 1000);
  while (dnsLookupInProgress) // Waiting for the DNS lookup to finish and dnsLookupResult callback to trigger
  {
    if (absolute_time_diff_us(Timeout, get_absolute_time()) > 0)
    {
      printf("DNS lookup timeout - %s\n", DnsName);
      return false;
    }
    vTaskDelay(pdMS_TO_TICKS(500));
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
    printf("Found address %s for %s\n", ipaddr_ntoa(FoundIP), Hostname);
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