#pragma once

#include "lwip/apps/mqtt.h"
#include "pico/cyw43_arch.h"

typedef void (*Callback_type)(const u8_t *Data, u16_t Len); // Defines how the DataCallback function in mqttConnect should look like

void mqttDisconnect(mqtt_client_t *Client);                                                                                                 ///< Disconnect from MQTT server
void mqttConnect(mqtt_client_t *Client, mqtt_connect_client_info_t *ClientInfo, ip_addr_t *ServerIP, u16_t ServerPort, void *DataCallback); ///< Initiate connection to the MQTT server
void mqttConnect_Callback(mqtt_client_t *Client, void *Arg, mqtt_connection_status_t Status);                                               ///< Callback with the connection result
bool mqttIsConnected(mqtt_client_t *Client);                                                                                                ///< true: Connected to MQTT server, false: not connected
void mqttPublish(mqtt_client_t *Client, const char *PubTopic, const char *PubData, u8_t QoS, bool Retain);                                  ///< Publish a message to the PubTopic
void mqttPublish_Callback(void *Arg, err_t Result);                                                                                         ///< Callback with the publish result
void mqttSubscribe_Unsubscribe(mqtt_client_t *Client, const char *SubTopic, u8_t QoS, bool Subscribe);                                      ///< bool Subscribe=true: Subscribe to a topic, bool Subscribe=false: Unsubscribe from a topic
void mqttSubscribe_Callback(void *Arg, err_t Result);                                                                                       ///< Callback with the subscription result: 0: Success
void mqttIncomingTopic_Callback(void *Arg, const char *Topic, u32_t Tot_len);                                                               ///< Callback when a new message is received on a subscribed topic: Gets the topic
void mqttIncomingData_Callback(void *Arg, const u8_t *Data, u16_t Len, u8_t Flags);                                                         ///< Callback when a new message is received on a subscribed topic: Gets the payload
