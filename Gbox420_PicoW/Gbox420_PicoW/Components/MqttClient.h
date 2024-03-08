#pragma once

#include "lwip/apps/mqtt.h"
#include "pico/cyw43_arch.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"
#include "lwip/err.h"
#include "../Settings.h" ///< for loading defaults and storing/reading user settings

#define MaxWordLength 128 ///< Default char * buffer length for storing a word + null terminator. Memory intense!

class MqttClient
{
    typedef void (*Callback_type)(const uint8_t *Data, uint16_t Len); // Defines how the DataCallback function in mqttConnect should look like

public:
    MqttClient(Settings::MqttClientSettings *DefaultSettings, void *DataCallback);
    void mqttConnect();                                                                 ///< Initiate connection to the MQTT server
    void mqttDisconnect();                                                              ///< Disconnect from MQTT server
    bool mqttIsConnected();                                                             ///< true: Connected to MQTT server, false: not connected
    void mqttPublish(const char *PubTopic, const char *PubData, uint8_t QoS, bool Retain); ///< Publish a message to the PubTopic
    void mqttSubscribe_Unsubscribe(const char *SubTopic, uint8_t QoS, bool Subscribe);     ///< bool Subscribe=true: Subscribe to a topic, bool Subscribe=false: Unsubscribe from a topic

private:
    bool dnsLookupInProgress; ///< If MQTTServerDNS is specified the IP address needs to be resolved. true: DNS lookup in progress

protected:
    mqtt_client_t *Client;
    mqtt_connect_client_info_t ClientInfo;
    ip_addr_t ServerIP;
    uint16_t MQTTServerPort;
    void *DataCallback;                                                                                  ///< Pointer to the callback function (Callback_type)
    static void mqttIpFound(const char *Hostname, const ip_addr_t *Ipaddr, void *Arg);                   ///< Called When the IP address of the MQTT server is found
    static void mqttPublish_Callback(void *Arg, err_t Result);                                           ///< Callback with the publish result
    static void mqttSubscribe_Callback(void *Arg, err_t Result);                                         ///< Callback with the subscription result: 0: Success
    static void mqttConnect_Callback(mqtt_client_t *Client, void *Arg, mqtt_connection_status_t Status); ///< Callback with the connection result
    static void mqttIncomingTopic_Callback(void *Arg, const char *Topic, uint32_t Tot_len);                 ///< Callback when a new message is received on a subscribed topic: Gets the topic
    static void mqttIncomingData_Callback(void *Arg, const uint8_t *Data, uint16_t Len, uint8_t Flags);           ///< Callback when a new message is received on a subscribed topic: Gets the payload
};