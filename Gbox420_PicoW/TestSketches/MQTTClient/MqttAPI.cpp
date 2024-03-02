#include "MqttAPI.h"

void mqttDisconnect(mqtt_client_t *Client)
{
    mqtt_disconnect(Client);
}

void mqttConnect(mqtt_client_t *Client, mqtt_connect_client_info_t *ClientInfo, ip_addr_t *ServerIP, u16_t ServerPort, void *DataCallback)
{
    cyw43_arch_lwip_begin();
    err_t err = mqtt_client_connect(Client, ServerIP, ServerPort, mqttConnect_Callback, DataCallback, ClientInfo);
    cyw43_arch_lwip_end();

    if (err != ERR_OK)
    {
        printf("mqttConnect error: %d\n", err);
    }
}

void mqttConnect_Callback(mqtt_client_t *Client, void *Arg, mqtt_connection_status_t Status)
{
    printf("mqttConnect_Callback: ");
    if (Status == MQTT_CONNECT_ACCEPTED)
    {
        printf("Successfully connected\n");
        mqtt_set_inpub_callback(Client, mqttIncomingTopic_Callback, mqttIncomingData_Callback, Arg);
    }
    else
    {
        printf("Disconnected, reason: %d\n", Status); // 0: Accepted, 1:Protocol version refused, 2:Identifier refused, 3:Server refused, 4:Credentials refused, 5:Not authorized, 256:MQTT Disconnect, 257:Timeout
        // mqttConnect(Client);  //TODO: Add auto-reconnect
    }
}

bool mqttIsConnected(mqtt_client_t *Client)
{
    if (mqtt_client_is_connected(Client) == 1)
        return true;
    else
        return false;
}

void mqttSubscribe_Unsubscribe(mqtt_client_t *Client, const char *SubTopic, u8_t QoS, bool Subscribe)
{
    if (Subscribe)
    {
        printf("Subscribing to %s ...", SubTopic);
    }
    else
    {
        printf("Unsubscribing from %s ...", SubTopic);
    }
    err_t err = mqtt_sub_unsub(Client, SubTopic, QoS, mqttSubscribe_Callback, NULL, Subscribe);
    if (err != ERR_OK)
    {
        printf("mqttSubscribe error: %d\n", err);
    }
}

void mqttSubscribe_Callback(void *Arg, err_t Result)
{
    if (Result == 0)
    {
        printf("done\n");
    }
    else
    {
        printf("failed: %s\n", Result);
    }
}

void mqttIncomingTopic_Callback(void *Arg, const char *Topic, u32_t Tot_len)
{
    printf("Incoming topic: %s ,total length: %u\n", Topic, (unsigned int)Tot_len);
}

void mqttIncomingData_Callback(void *Arg, const u8_t *Data, u16_t Len, u8_t Flags)
{
    printf("Incoming payload with length %d, flags %u\n", Len, (unsigned int)Flags);

    if (Flags == 1) // Last fragment of payload received (or the whole payload fits receive buffer (MQTT_VAR_HEADER_BUFFER_LEN, MQTT_DATA_FLAG_LAST)
    {
        ((Callback_type)Arg)(Data, Len);
    }
    else
    {
        printf("Max payload exceeded"); /// TODO: Implement multiple package handling
    }
}

void mqttPublish(mqtt_client_t *Client, const char *PubTopic, const char *PubData, u8_t QoS, bool Retain)
{
    printf("Publishing data to %s ...", PubTopic);
    err_t err = mqtt_publish(Client, PubTopic, PubData, strlen(PubData), QoS, Retain, mqttPublish_Callback, &PubTopic);
    if (err != ERR_OK)
    {
        printf("Publish err: %d\n", err);
    }
}

void mqttPublish_Callback(void *Arg, err_t Result)
{
    if (Result != ERR_OK)
    {
        printf("Publish error: %d\n", Result);
    }
    else
    {
        printf("done\n");
    }
}