/*! \file
 *  \brief     WiFi connection test using an NTP query to update the built-in Real Time Clock
 *  \details   
 *  \attention 
 *             update the WIFI_SSID and WIFI_PASSWORD in the Global constants section
 *             run the builder.bat
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/raspberry-pi-pico-w
 *  \version   4.20
 */

#pragma GCC diagnostic ignored "-Wstringop-truncation"  //Surpress build warning of potential truncation of a string NULL terminator. During string operations the size is pre-defined in Settings.h: MaxWordLength/MaxShotTextLength/MaxLongTextLength 
#include "hardware/rtc.h"   //Real Time Clock for storing current time (Updated over NTP)
#include "hardware/watchdog.h" //Watchdog to auto-reboot in case of an error
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/util/datetime.h"
#include "Settings.h"                         // Settings for every component //TODO: Try passing these directly when creating an object?
#include "Components/NTPClient.h"
#include "Components/Sound.h"
#include "Components/Hempy_Standalone.h" // Represents a complete box with all feautres


// Global variable initialization
datetime_t CurrentDateTime;   ///<   calling getRTC() function it will update this variable
char CurrentTime[MaxWordLength] = "";      ///< calling getRTC() function it will update this variable   //TODO: Remove this, return it instead in getRTC()
char LongMessage[MaxLongTextLength] = "";  ///< Temp storage for assembling long messages (REST API, MQTT reporting)
char ShortMessage[MaxShotTextLength] = ""; ///< Temp storage for assembling short messages (Log entries, Error messages)
bool *Debug;                              ///< True - Turns on extra debug messages on the Serial Output
bool *Metric;                             ///< True - Use metric units, False - Use imperial units
bool MqttConnected = false;               ///< Track the connection state to the MQTT broker configured on the ESP-link's REST/MQTT tab

// Component initialization/declaration
/*ELClient ESPLink(&ESPSerial);             ///< ESP-link. Both SLIP and debug messages are sent to ESP over the ESP Serial link
ELClientWebServer WebServer(&ESPLink);    ///< ESP-link - WebServer API
ELClientCmd ESPCmd(&ESPLink);             ///< ESP-link - Helps getting the current time from the internet using NTP
ELClientRest PushingBoxRestAPI(&ESPLink); ///< ESP-link - REST API
ELClientMqtt MqttAPI(&ESPLink);           ///< ESP-link - MQTT protocol for sending and receiving messages
*/
Settings *ModuleSettings;                 ///< This object will store the settings loaded from the EEPROM. Persistent between reboots. //TODO: Find a solution to store this (NO EEPROM on Pico)
Hempy_Standalone *Hempy_Standalone1;      ///< Represents a Grow Box with all components (LED lights, DHT sensors, Power sensor, Hempy Buckets, Water pants..etc)

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
            printf("...failed\n");
        }
        else
            printf("...done\n");
    }
}

// Query current time from local RTC
void getRTC()
{
    rtc_get_datetime(&CurrentDateTime);
    datetime_to_str(ShortMessage, MaxShotTextLength, &CurrentDateTime);
    printf("%s", ShortMessage);
}

int main()
{
    stdio_init_all();
    if (watchdog_caused_reboot()) {
        printf("\nRebooted by Watchdog!\n");
    } else {
        printf("\nClean boot\n");
    }  
    printf("\nHempy module initializing\n");
    watchdog_enable(0x7fffff, 1);  // Maximum of 0x7fffff, which is approximately 8.3 seconds
    
    initializeWiFi();
    initializeRTC();
    
      // Loading settings from EEPROM
  printf("Loading settings\n");
  ModuleSettings = loadSettings();
  Debug = &ModuleSettings->Debug;
  Metric = &ModuleSettings->Metric;

  if ((ModuleSettings->Hempy_Standalone1).ReportToMQTT)
  {
    //setupMqtt(); //MQTT message relay setup. Logs "ConnectedCB is XXXX" to serial if successful
  }

  // Create the Module objects
   printf("Creating Hempy module\n");
    Hempy_Standalone1 = new Hempy_Standalone("Hemp1", &ModuleSettings->Hempy_Standalone1); ///< This is the dev object representing an entire Grow Box with all components in it. Receives its name and the settings loaded from the EEPROM as parameters

  //   sendEmailAlert("Grow%20box%20(re)started");
  printf("Setup ready, starting loops:\n");


    // run_ntp_test();
    int LoopCount = 0;
    absolute_time_t LastRefresh = get_absolute_time();
    while (LoopCount < 10)
    {
        if (absolute_time_diff_us(LastRefresh, get_absolute_time()) > 5000000) // 5sec
        {
            watchdog_update();
            getRTC();
            printf(" Running...\n");
            LastRefresh = get_absolute_time();
            LoopCount++;
        }
    }
    cyw43_arch_deinit();
    return 0;
}


/**
  \brief Sets up the MQTT relay: Configures callbacks for MQTT events and sets the Last Will and Testament in case the ESP-link goes offline 
*/
void setupMqtt()
{
/*
  MqttAPI.connectedCb.attach(mqttConnected);
  MqttAPI.disconnectedCb.attach(mqttDisconnected);
  MqttAPI.publishedCb.attach(mqttPublished);
  MqttAPI.dataCb.attach(mqttReceived);

  memset(&ShortMessage[0], 0, MaxShotTextLength); //reset variable to store the Publish to path
  strcat(ShortMessage, ModuleSettings->MqttLwtTopic);
  MqttAPI.lwt(ShortMessage, ModuleSettings->MqttLwtMessage, 0, 1); //(topic,message,qos,retain) declares what message should be sent on it's behalf by the broker after Gbox420 has gone offline.
  MqttAPI.setup();
  */
}

/**
  \brief Called when connection to the MQTT broker is established
*/
void mqttConnected(__attribute__((unused)) void *response)
{
 // MqttAPI.subscribe(ModuleSettings->MqttSubTopic);
  MqttConnected = true;
  //if(*Debug) printf("MQTT connected\n");
}

/**
  \brief Called when connection to the MQTT broker is lost
*/
void mqttDisconnected(__attribute__((unused)) void *response)
{
  MqttConnected = false;
  //if(*Debug) printf("MQTT disconnected\n");
}

/**
  \brief Called after an MQTT message is sent out - Not used
*/
void mqttPublished(__attribute__((unused)) void *response)
{
  //if(*Debug) printf("MQTT published\n");
}

/**
  \brief Called when an MQTT command is received. Extracts the command from the MQTT topic and gets the data passed along the command
  \param response The MQTT message object passed by ESP-link (type: ELClientResponse)
*/
void mqttReceived(void *response)
{
  //static uint8_t MqttSubTopicLength = strlen(ModuleSettings->MqttSubTopic) - 1; //Get length of the command topic
  static char command[MaxShotTextLength];
  static char data[MaxShotTextLength];
  //ELClientResponse *res = (ELClientResponse *)response;
  //String mqttTopic = (*res).popString();
  //String mqttData = (*res).popString();
  printf("MQTT\n");
//  printf(" %s",&mqttTopic);
  //mqttTopic.remove(0, MqttSubTopicLength); //Cut the known command topic from the arrived topic
  //mqttTopic.toCharArray(command, MaxShotTextLength);
  //mqttData.toCharArray(data, MaxShotTextLength);
  Hempy_Standalone1->commandEventTrigger(command, data);
  Hempy_Standalone1->reportToMQTTTrigger(true); //send out a fresh report
}
