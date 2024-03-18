/*!
 *  \brief     MQTT client 
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

#pragma once

#include <functional>
#include "lwip/apps/mqtt.h"
#include "pico/cyw43_arch.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"
#include "lwip/err.h"
#include "../Settings.h" ///< for loading defaults and storing/reading user settings
#include "Helpers.h"
#include "420Common.h"
#include "420Module.h"

#define MaxWordLength 128                                  ///< Default char * buffer length for storing a word + null terminator. Memory intense!
typedef std::function<void(char *, char *)> Callback_type; // Defines how the DataCallback function in mqttConnect should look like

class MqttClient : virtual public Common
{

public:
    MqttClient(Module *Parent, Settings::MqttClientSettings *DefaultSettings, Callback_type DataCallback);
    void report(bool FriendlyFormat = false);
    void run1min();
    void mqttConnect();                                                                ///< Initiate connection to the MQTT server
    void mqttCheck();                                                                  ///< Check connected status - Reconnect to MQTT server in the background if disconnected
    void mqttDisconnect();                                                             ///< Disconnect from MQTT server
    bool mqttIsConnected();                                                            ///< true: Connected to MQTT server, false: not connected
    char *mqttIsConnectedText(bool FriendlyFormat = false);                            ///< true: Connected to MQTT server, false: not connected
    void mqttPublish(char *PubTopic, char *PubData);                                   ///< Publish a message to the PubTopic
    void mqttSubscribe(); ///< bool Subscribe=true: Subscribe to a topic, bool Subscribe=false: Unsubscribe from a topic
    void mqttUnsubscribe(); ///< bool Subscribe=true: Subscribe to a topic, bool Subscribe=false: Unsubscribe from a topic
    char *SubTopic;      ///< Subscribe topic
    char *PubTopic;      ///< Topic to publish messages
    bool *PublishRetain; ///< Should the MQTT server retain Publish messages: 0:No retention (recommended), 1:Broker keeps the message and sends it to future subscribers

private:
    bool InProgress_ConnectAndSubscribe = false;
    bool InProgress_Publish = false;

protected:
    mqtt_client_t *Client;
    mqtt_connect_client_info_t *ClientInfo;
    char *MqttServerIP = NULL;
    char *MqttServerDNS = NULL;
    ip_addr_t MqttServerAddress;
    uint16_t *MqttServerPort;
    uint8_t *QoS=NULL;
    char LastReceivedTopic[MaxShotTextLength];
    Callback_type DataCallback;                                                                          ///< Pointer to the callback function (Callback_type)
    static void mqttIpFound(const char *Hostname, const ip_addr_t *Ipaddr, void *Arg);                   ///< Called When the IP address of the MQTT server is found
    static void mqttPublish_Callback(void *Arg, err_t Result);                                           ///< Callback with the publish result
    static void mqttSubscribe_Callback(void *Arg, err_t Result);                                         ///< Callback with the subscription result: 0: Success
    static void mqttConnect_Callback(mqtt_client_t *Client, void *Arg, mqtt_connection_status_t Status); ///< Callback with the connection result
    static void mqttIncomingTopic_Callback(void *Arg, const char *Topic, uint32_t Tot_len);              ///< Callback when a new message is received on a subscribed topic: Gets the topic
    static void mqttIncomingData_Callback(void *Arg, const uint8_t *Data, uint16_t Len, uint8_t Flags);  ///< Callback when a new message is received on a subscribed topic: Gets the payload
};