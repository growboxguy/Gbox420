#include "MqttTestMulti.h"

int main()
{
    stdio_init_all();
    printf("\nStarting 10 sec delay...\n"); // Not needed, gives time to open a Serial Monitor
    sleep_ms(10000);                        // Not needed, gives time to open a Serial Monitor
    initializeWiFi();                       // Connect to the WiFi network

    Server1 = new MqttClient(&MQTTServer1, (void *)mqttReceivedData); // Create MQTT client object. pass the settings struct and the callback function's address
    Server2 = new MqttClient(&MQTTServer2, (void *)mqttReceivedData); // Create MQTT client object. and pass the settings struct and the callback function's address

    absolute_time_t LastRefresh = get_absolute_time(); // Used to track timeouts
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

            Server1->mqttPublish(MQTTServer1.PubTopic, FakeDataJSON, MQTTServer1.QoS, MQTTServer1.PublishRetain); // Publish the fake JSON to the MQTT server under PubTopic
            Server2->mqttPublish(MQTTServer2.PubTopic, FakeDataJSON, MQTTServer2.QoS, MQTTServer2.PublishRetain); // Publish the fake JSON to the MQTT server under PubTopic

            LastRefresh = get_absolute_time();
        }
    }

    // This section will never be reached
    Server1->mqttSubscribe_Unsubscribe(MQTTServer1.SubTopic, MQTTServer1.QoS, false); // Unsubscribe
    Server1->mqttDisconnect();                                                        // Disconnect from the MQTT server
    Server2->mqttSubscribe_Unsubscribe(MQTTServer2.SubTopic, MQTTServer2.QoS, false); // Unsubscribe
    Server2->mqttDisconnect();                                                        // Disconnect from the MQTT server
    cyw43_arch_deinit();
    return 0;
}

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

void mqttReceivedData(const u8_t *Data, u16_t Len)
{
    printf("%s\n", Data); // Print the message received on the subscribed topic
}

char *intToChar(int Number) // Converting int to char
{
    static char ReturnChar[8] = ""; // 7 digits + null terminator max
    itoa(Number, ReturnChar, 10);
    return ReturnChar;
}