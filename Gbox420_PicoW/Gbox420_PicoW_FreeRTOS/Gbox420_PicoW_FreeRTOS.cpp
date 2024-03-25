#include "Gbox420_PicoW_FreeRTOS.h"

// Initialize WiFi and Connect to local network
void connectWiFi()
{
  cyw43_arch_enable_sta_mode();                                                                                         // Enables Wi-Fi STA (Station) mode
  int WifiConnectResult = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000); // Max 10sec
  if (WifiConnectResult != 0)
  {
    printf("Connecting to %d failed: \n", WifiConnectResult);
  }
  else
    printf("Connected to %s\n", WIFI_SSID);
}

void wifiChecker(void *pvParameters)
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

void rtcChecker(void *pvParameters)
{
  printf("Initializing RTC...\n");
  // NtpClient1 = new NtpClient(&GboxSettings->NTPServer1);                     // TODO: Auto NTP update time every 24h
  while (1)
  {
    vTaskDelay(10000 / portTICK_PERIOD_MS); // Delay for 10sec
  }
}

void watchdog(void *pvParameters)
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

///< Blink built-in LED on Pico W - Requires WiFi initialization cyw43_arch_init() , since the LED is connected to the WiFi controller
void heartbeat(void *pvParameters)
{
  while (1)
  {
    vTaskDelay(1000);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, ledStatus);
    ledStatus = !ledStatus;
  }
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
  GboxSettings = loadSettings(false);
  Debug = &GboxSettings->Debug;
  Metric = &GboxSettings->Metric;
  printf("Creating tasks\n");
  xTaskCreate(watchdog, "Watchdog", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L3, NULL);        ///< Blink built-in LED
  xTaskCreate(wifiChecker, "WiFi checker", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L2, NULL); ///< Connect-ReConnect to WiFi
  xTaskCreate(rtcChecker, "RTC update", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L2, NULL);    ///< Update Real Time Clock using NTP
  xTaskCreate(heartbeat, "Heartbeat", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_L1, NULL);      ///< Blink built-in LED
  printf("Starting scheduler\n");
  vTaskStartScheduler();

  cyw43_arch_deinit();
  return 0;
}
