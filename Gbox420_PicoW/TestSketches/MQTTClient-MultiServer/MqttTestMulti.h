#pragma once

#include "pico/cyw43_arch.h"
#include "pico/stdio.h" 

#include "MqttAPI.h"

struct MQTTClientSettings MQTTServer1 = {.MQTTServerDNS = "mqttserver.gbox420.net", .MQTTServerIP = "", .MQTTServerPort = 1883, .MQTTServerUser = "MqttUser", .MQTTServerPassword = "SuperSecretPassword", .ClientID = "Hempy", .PubTopic = "Gbox420/Hempy/", .SubTopic = "Gbox420CMD/Hempy/#", .LwtTopic = "Gbox420LWT/Hempy/", .LwtMessage = "Hempy Offline", .LwtRetain = true, .PublishRetain = true, .QoS = 1, .KeepAliveSeconds = 30};
struct MQTTClientSettings MQTTServer2 = {.MQTTServerDNS = "", .MQTTServerIP = "192.168.1.140", .MQTTServerPort = 1883, .MQTTServerUser = "MqttUser", .MQTTServerPassword = "Abc123", .ClientID = "Hempy2", .PubTopic = "Gbox420/Hempy2/", .SubTopic = "Gbox420CMD/Hempy2/#", .LwtTopic = "Gbox420LWT/Hempy2/", .LwtMessage = "Hempy2 Offline", .LwtRetain = true, .PublishRetain = true, .QoS = 1, .KeepAliveSeconds = 30};

int main();                                                                 ///< Test logic: Initialize WiFi -> Resolve MQTT server IP -> Connect to MQTT server -> Subscribe to a test topic (defined by SubTopic) -> Publish a fake JSON to an MQTT topic (defined by PubTopic)
void initializeWiFi();                                                      ///< Connect to the WiFi network
void mqttReceivedData(const u8_t *Data, u16_t Len);                         ///< Prints MQTT data received from the Subscribed topic
char *intToChar(int Number);                                                ///< Used when generating the fake JSON data to Publish

MqttAPI *Server1 = NULL;
MqttAPI *Server2 = NULL;
