#pragma once

#include "lwip/apps/mqtt.h"
#include "pico/cyw43_arch.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"
#include "lwip/err.h"

#define MaxWordLength 128 ///< Default char * buffer length for storing a word + null terminator. Memory intense!

struct MQTTClientSettings ///< MQTT client settings
{
    char MQTTServerDNS[MaxWordLength];      ///< MQTT server DNS name, "" to use MQTTServerIP instead
    char MQTTServerIP[MaxWordLength];       ///< MQTT server IP. Used when MQTTServerDNS is empty, or the DNS lookup fails
    u16_t MQTTServerPort;                   ///< MQTT server Port
    char MQTTServerUser[MaxWordLength];     ///< MQTT server username, "" if not needed
    char MQTTServerPassword[MaxWordLength]; ///< MQTT server password, "" if not needed
    char ClientID[MaxWordLength];           ///< Should be unique across the MQTT server
    char PubTopic[MaxWordLength];           ///< Publish MQTT messages to this topic. Ends with a forward slash/
    char SubTopic[MaxWordLength];           ///< Subscribe to messages of this topic and all sub-topics
    char LwtTopic[MaxWordLength];           ///< Last Will and Testament topic: When the connection is lost the MQTT broker will publish a final message to this topic. Ends with a forward slash
    char LwtMessage[MaxWordLength];         ///< Last Will and Testament message: Subscribers will get this message under the topic specified by LwtTopic when the MQTT client goes offline
    bool LwtRetain;                         ///< Last Will and Testament retention: 0:No retention, 1:Broker keeps the message and sends it to future subscribers (recommended)
    bool PublishRetain;                     ///< Should the MQTT server retain Publish messages: 0:No retention (recommended), 1:Broker keeps the message and sends it to future subscribers
    uint8_t QoS;                            ///< Quality of Service levels: 0:No QoS, 1: Broker ensures to send the message to the subscribers (recommended), 2: Broker ensures to send the message to the subscribers exactly once   https://www.hivemq.com/blog/mqtt-essentials-part-6-mqtt-quality-of-service-levels/
    uint32_t KeepAliveSeconds;              ///< Ping the MQTT server every X seconds to keep the connection active
};

class MqttAPI
{
    typedef void (*Callback_type)(const u8_t *Data, u16_t Len); // Defines how the DataCallback function in mqttConnect should look like

public:
    MqttAPI(MQTTClientSettings *DefaultSettings, void *DataCallback);
    void mqttConnect();                                                                 ///< Initiate connection to the MQTT server
    void mqttDisconnect();                                                              ///< Disconnect from MQTT server
    bool mqttIsConnected();                                                             ///< true: Connected to MQTT server, false: not connected
    void mqttPublish(const char *PubTopic, const char *PubData, u8_t QoS, bool Retain); ///< Publish a message to the PubTopic
    void mqttSubscribe_Unsubscribe(const char *SubTopic, u8_t QoS, bool Subscribe);     ///< bool Subscribe=true: Subscribe to a topic, bool Subscribe=false: Unsubscribe from a topic

private:
    bool dnsLookupInProgress; ///< If MQTTServerDNS is specified the IP address needs to be resolved. true: DNS lookup in progress

protected:
    mqtt_client_t *Client;
    mqtt_connect_client_info_t ClientInfo;
    ip_addr_t ServerIP;
    u16_t MQTTServerPort;
    void *DataCallback;                                                                                  ///< Pointer to the callback function (Callback_type)
    static void mqttIpFound(const char *Hostname, const ip_addr_t *Ipaddr, void *Arg);                   ///< Called When the IP address of the MQTT server is found
    static void mqttPublish_Callback(void *Arg, err_t Result);                                           ///< Callback with the publish result
    static void mqttSubscribe_Callback(void *Arg, err_t Result);                                         ///< Callback with the subscription result: 0: Success
    static void mqttConnect_Callback(mqtt_client_t *Client, void *Arg, mqtt_connection_status_t Status); ///< Callback with the connection result
    static void mqttIncomingTopic_Callback(void *Arg, const char *Topic, u32_t Tot_len);                 ///< Callback when a new message is received on a subscribed topic: Gets the topic
    static void mqttIncomingData_Callback(void *Arg, const u8_t *Data, u16_t Len, u8_t Flags);           ///< Callback when a new message is received on a subscribed topic: Gets the payload
};