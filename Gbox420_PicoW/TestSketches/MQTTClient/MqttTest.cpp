/*! \file
 *  \brief     MQTT connection test - Pico W
 *  \details   Connect to an MQTT server using the LwIP MQTT Client (https://github.com/stm32duino/LwIP/blob/main/src/lwip/apps/mqtt.h , https://www.nongnu.org/lwip/2_0_x/group__mqtt.html , https://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html)
 *  \attention Update the Constants section
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/raspberry-pi-pico-w
 *  \version   4.20
 */

#include "pico/cyw43_arch.h"
#include "pico/stdio.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"
#include "MqttTest.h"
#include "MqttAPI.h"

// Constants - UPDATE THIS SECTION
#define WIFI_SSID "GboxNet"                      ///< WiFi SSID
#define WIFI_PASSWORD "SuperSecretPassword"      ///< WiFi Password
#define MQTTServerDNS NULL                       ///< MQTT server DNS name, leave NULL to use MQTTServerIP instead
#define MQTTServerIP "192.168.1.100"             ///< MQTT server IP
#define MQTTServerPort 1883                      ///< MQTT server Port
#define MQTTServerUser "MqttUser"                ///< MQTT server username, NULL if not needed
#define MQTTServerPassword "SuperSecretPassword" ///< MQTT server password, NULL if not needed
#define ClientID "Hempy"                         ///< Should be unique across the MQTT server
#define PubTopic "Gbox420/Hempy/"                ///< Publish MQTT messages to this topic. Ends with a forward slash/
#define SubTopic "Gbox420CMD/Hempy/#"            ///< Subscribe to messages of this topic and all sub-topics
#define LwtTopic "Gbox420LWT/Hempy/"             ///< Last Will and Testament topic: When the connection is lost the MQTT broker will publish a final message to this topic. Ends with a forward slash
#define LwtMessage "Hempy Offline"               ///< Last Will and Testament message: Subscribers will get this message under the topic specified by LwtTopic when the MQTT client goes offline
#define LwtRetain 1                              ///< Last Will and Testament retention: 0:No retention, 1:Broker keeps the message and sends it to future subscribers (recommended)
#define PublishRetain 0                          ///< Should the MQTT server retain Publish messages: 0:No retention (recommended), 1:Broker keeps the message and sends it to future subscribers
#define QoS 1                                    ///< Quality of Service levels: 0:No QoS, 1: Broker ensures to send the message to the subscribers (recommended), 2: Broker ensures to send the message to the subscribers exactly once   https://www.hivemq.com/blog/mqtt-essentials-part-6-mqtt-quality-of-service-levels/
#define KeepAliveSeconds 30                      ///< Ping the MQTT broker every X seconds. Else the broker executes the Last Will and Testament

bool dnsLookupInProgress = false; ///< If MQTTServerDNS is specified the IP address needs to be resolved. true: DNS lookup in progress
ip_addr_t ServerIP;               ///< Stores the MQTT server's IP address. Filled either by the DNS lookup result or using MQTTServerIP

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
        printf("Connecting to %s...", WIFI_SSID);
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)) // 10sec timeout
        {
            printf("failed\n");
        }
        else
            printf("done\n");
    }
}

int main()
{
    stdio_init_all();
    printf("\nStarting 10 sec delay...\n"); // Not needed, gives time to open a Serial Monitor
    sleep_ms(10000);                        // Not needed, gives time to open a Serial Monitor
    initializeWiFi();                       // Connect to the WiFi network

    absolute_time_t LastRefresh = get_absolute_time(); // Used to track timeouts

    if (MQTTServerDNS != NULL) // If an MQTT server DNS name is specified -> Look up the IP
    {
        printf("Looking up IP for %s...", MQTTServerDNS);
        err_t err = dns_gethostbyname(MQTTServerDNS, &ServerIP, mqttIpFound, NULL);
        if (err == ERR_OK) // DNS name found in cache
        {
            printf("Found cached address\n");
        }
        else
        {
            dnsLookupInProgress = true;
        }

        while (dnsLookupInProgress) // Waiting for the DNS lookup to finish
        {
            sleep_ms(100);
            if (absolute_time_diff_us(LastRefresh, get_absolute_time()) > 10000000) // 10sec timeout
            {
                printf("DNS lookup timeout\n");
                break;
            }
        }
    }
    else
    {
        ip4addr_aton(MQTTServerIP, &ServerIP); // DNS not specified, use MQTTServerIP instead
    }

    mqtt_client_t *Client = mqtt_client_new();    // Allocate memory for the Stuct storing the MQTT client, store the pointer to it
    struct mqtt_connect_client_info_t ClientInfo; // Stuct for storing the MQTT connection parameters (mqtt_connect_client_info_t is defined by LWIP)
    memset(&ClientInfo, 0, sizeof(ClientInfo));   // Allocate memory for the ClientInfo
    ClientInfo.client_id = ClientID;
    ClientInfo.client_user = MQTTServerUser;
    ClientInfo.client_pass = MQTTServerPassword;
    ClientInfo.keep_alive = KeepAliveSeconds;
    ClientInfo.will_topic = LwtTopic;
    ClientInfo.will_msg = LwtMessage;
    ClientInfo.will_qos = QoS;
    ClientInfo.will_retain = LwtRetain;

    mqttConnect(Client, &ClientInfo, &ServerIP, MQTTServerPort, (void *)mqttReceivedData); // Start the connection process to the MQTT server. mqttReceivedData: Pass as function pointer, used then an MQTT subscription receives data
    LastRefresh = get_absolute_time();                                                     // Reset the timeout counter
    while (!mqttIsConnected(Client))                                                       // Waiting for the MQTT connection to establish
    {
        sleep_ms(100);
        if (absolute_time_diff_us(LastRefresh, get_absolute_time()) > 10000000) // 10sec timeout
        {
            printf("MQTT connect timeout\n");
            break;
        }
    }
    mqttSubscribe_Unsubscribe(Client, SubTopic, QoS, true); // Subscribe to MQTT messages in the SubTopic topic.
    printf("Listening on topic: %s\n", SubTopic);

    while (1)
    {
        if (absolute_time_diff_us(LastRefresh, get_absolute_time()) > 30000000) // 30sec
        {
            char FakeDataJSON[128];                            // Generate a fake JSON to Publish
            memset(&FakeDataJSON[0], 0, sizeof(FakeDataJSON)); // clear variable
            strcat(FakeDataJSON, "{\"Humidity\":\"");
            strcat(FakeDataJSON, intToChar(rand() % 100)); // random int 0-99
            strcat(FakeDataJSON, "\",\"Speed\":\"");
            strcat(FakeDataJSON, intToChar(rand() % 10)); // random int 0-9
            strcat(FakeDataJSON, "\",\"LightOn\":\"");
            strcat(FakeDataJSON, intToChar(rand() % 2)); // random 0 or 1
            strcat(FakeDataJSON, "\"}");                 // closing the JSON
            printf("Fakedata: %s\n", FakeDataJSON);
            mqttPublish(Client, PubTopic, FakeDataJSON, QoS, PublishRetain); // Publish the fake JSON to the MQTT server under PubTopic
            LastRefresh = get_absolute_time();
        }
    }

    // This section will never be reached
    mqttSubscribe_Unsubscribe(Client, SubTopic, QoS, false); // Unsubscribe
    mqttDisconnect(Client);                                  // Disconnect from the MQTT server
    cyw43_arch_deinit();
    return 0;
}

void mqttReceivedData(const u8_t *Data, u16_t Len)
{
    printf("%s\n", Data); // Print the message received on the subscribed topic
}

void mqttIpFound(const char *Hostname, const ip_addr_t *Ipaddr, void *Arg) // DNS lookup callback
{
    dnsLookupInProgress = false;
    if (Ipaddr)
    {
        printf("Found address: %s\n", ipaddr_ntoa(Ipaddr));
        ServerIP = *Ipaddr;
    }
    else
    {
        printf("DNS lookup failed\n");
    }
}

char *intToChar(int Number) // Converting int to char
{
    static char ReturnChar[8] = ""; // 7 digits + null terminator max
    itoa(Number, ReturnChar, 10);
    return ReturnChar;
}