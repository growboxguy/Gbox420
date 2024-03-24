#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Settings.h"

#define RUN_FREERTOS_ON_CORE 0
#define TASK_PRIORITY_L1 (tskIDLE_PRIORITY + 1UL) // Lower means less important
#define TASK_PRIORITY_L2 (tskIDLE_PRIORITY + 2UL) // Higher means more important

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
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)) // Max 10sec
    {
      printf("...failed\n");
    }
    else
      printf("...done\n");
  }
}

///< Blink built-in LED on Pico W - Requires WiFi initialization cyw43_arch_init() , since the LED is connected to the WiFi controller
bool ledStatus = false;
void heartbeat(void *pvParameters)
{
  while (1)
  {
    vTaskDelay(1000);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, ledStatus);    
    ledStatus = !ledStatus;
  }
}

void wifiChecker(void *pvParameters)
{
  initializeWiFi();
  while (1)
  {
    vTaskDelay(30000 / portTICK_PERIOD_MS);                                           // Delay for 30sec
    printf("WiFi status:%d\n", cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA)); // Returns the status of the wifi link: CYW43_LINK_DOWN(0)-link is down,CYW43_LINK_JOIN(1)-Connected to WiFi,CYW43_LINK_NOIP(2)-Connected to WiFi, but no IP address,CYW43_LINK_UP  (3)-Connect to WiFi with an IP address,CYW43_LINK_FAIL(-1)-Connection failed,CYW43_LINK_NONET(-2)-No matching SSID found (could be out of range, or down),CYW43_LINK_BADAUTH(-3)-Authentication failure
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP)
    {
      cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK); // Try connecting again in the background
      printf("Reconnecting WiFi: %s\n", WIFI_SSID);
    };
  }
}

int main()
{
  stdio_init_all();
  printf("\nGbox420 initializing\n");
  printf("Creating tasks...\n");
  xTaskCreate(wifiChecker, "WiFi checker", 1024, NULL, TASK_PRIORITY_L2, NULL);
  xTaskCreate(heartbeat, "Heartbeat", 1024, NULL, TASK_PRIORITY_L1, NULL);
  printf("Starting scheduler...\n");
  vTaskStartScheduler();
  return 0;
}
