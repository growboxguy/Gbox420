#include "MqttClient.h"

MqttClient::MqttClient(Settings::MqttClientSettings *DefaultSettings, void *DataCallback)
{
    this->DataCallback = DataCallback;
    MqttServerPort = &DefaultSettings->MqttServerPort;
    Client = mqtt_client_new();                 // Allocate memory for the Stuct storing the MQTT client, store the pointer to it
    memset(&ClientInfo, 0, sizeof(ClientInfo)); // Allocate memory for the ClientInfo
    ClientInfo.client_id = DefaultSettings->ClientID;
    ClientInfo.client_user = DefaultSettings->MqttServerUser;
    ClientInfo.client_pass = DefaultSettings->MqttServerPassword;
    ClientInfo.keep_alive = DefaultSettings->KeepAliveSeconds;
    ClientInfo.will_topic = DefaultSettings->LwtTopic;
    ClientInfo.will_msg = DefaultSettings->LwtMessage;
    ClientInfo.will_qos = DefaultSettings->QoS;
    ClientInfo.will_retain = DefaultSettings->LwtRetain;

    ip4addr_aton(DefaultSettings->MqttServerIP, &MqttServerAddress); // If MqttServerDNS is defined and the DNS lookup is successful this will be overwritten
    if (DefaultSettings->MqttServerDNS[0] != '\0')                   // If an MQTT server DNS name is specified -> Look up the IP
    {
        DnsLookup(DefaultSettings->MqttServerDNS, &MqttServerAddress);
    }

    absolute_time_t NextRefresh = make_timeout_time_ms(10000); // 10sec - Used to track timeouts
    mqttConnect();
    while (!mqttIsConnected()) // Waiting for the MQTT connection to establish
    {
        if (get_absolute_time() > NextRefresh) // 10sec timeout
        {
            printf("MQTT connect timeout\n");
            break;
        }
        sleep_ms(100);
    }

    if (DefaultSettings->SubTopic[0] != '\0' && mqttIsConnected()) // If a SubTopic was configured -> Subscribe to it
    {
        NextRefresh = make_timeout_time_ms(5000); // 5sec
        SubscribeInProgress = true;
        mqttSubscribe_Unsubscribe(DefaultSettings->SubTopic, DefaultSettings->QoS, true); // Subscribe to MQTT messages in the SubTopic topic.

        while (SubscribeInProgress)
        {
            if (get_absolute_time() > NextRefresh) // 10sec timeout
            {
                printf("MQTT connect timeout\n");
                break;
            }
            sleep_ms(100);
        }
    }
}

void MqttClient::mqttConnect()
{
    printf("Connecting to MQTT server...");
    cyw43_arch_lwip_begin();
    err_t err = mqtt_client_connect(Client, &MqttServerAddress, *MqttServerPort, mqttConnect_Callback, this, &ClientInfo);
    cyw43_arch_lwip_end();

    if (err != ERR_OK)
    {
        printf("error: %d\n", err);
    }
    else
    {
        printf("initiated, waiting for callback...");
    }
}

void MqttClient::mqttDisconnect()
{
    mqtt_disconnect(Client);
}

void MqttClient::mqttConnect_Callback(mqtt_client_t *Client, void *Arg, mqtt_connection_status_t Status)
{
    if (Status == MQTT_CONNECT_ACCEPTED)
    {
        printf("Connected\n");
        mqtt_set_inpub_callback(Client, mqttIncomingTopic_Callback, mqttIncomingData_Callback, Arg);
    }
    else
    {
        printf("Disconnected, reason: %d\n", Status); // 0: Accepted, 1:Protocol version refused, 2:Identifier refused, 3:Server refused, 4:Credentials refused, 5:Not authorized, 256:MQTT Disconnect, 257:Timeout
        // mqttConnect(Client);  //TODO: Add auto-reconnect
    }
}

bool MqttClient::mqttIsConnected()
{
    if (mqtt_client_is_connected(Client) == 1)
        return true;
    else
        return false;
}

void MqttClient::mqttSubscribe_Unsubscribe(const char *SubTopic, uint8_t QoS, bool Subscribe)
{
    if (Subscribe)
    {
        printf("Subscribing to %s...", SubTopic);
    }
    else
    {
        printf("Unsubscribing from %s...", SubTopic);
    }
    err_t err = mqtt_sub_unsub(Client, SubTopic, QoS, mqttSubscribe_Callback, this, Subscribe);
    if (err != ERR_OK)
    {
        printf("error: %d\n", err);
    }
}

void MqttClient::mqttSubscribe_Callback(void *Arg, err_t Result)
{
    if (Result == 0)
    {
        printf("done\n");
    }
    else
    {
        printf("failed: %s\n", Result);
    }
    ((MqttClient *)Arg)->SubscribeInProgress = false;
}

void MqttClient::mqttIncomingTopic_Callback(void *Arg, const char *Topic, uint32_t Tot_len)
{
    strcpy(((MqttClient *)Arg)->LastReceivedTopic, Topic);
    // printf("MQTT incoming topic: %s\n", Topic);
}

void MqttClient::mqttIncomingData_Callback(void *Arg, const uint8_t *Data, uint16_t Len, uint8_t Flags)
{
    char TempData[Len + 1] = {}; // The +1 character is the null terminator
    memcpy(TempData, Data, Len);
    printf("MQTT incoming[%u] - Topic[%zu]: %s - Payload[%d]: %s\n", (unsigned int)Flags, strlen(((MqttClient *)Arg)->LastReceivedTopic), ((MqttClient *)Arg)->LastReceivedTopic, Len, TempData);

    if (Flags == 1) // Last fragment of payload received (or the whole payload fits receive buffer (MQTT_VAR_HEADER_BUFFER_LEN, MQTT_DATA_FLAG_LAST)
    {
        ((Callback_type)((MqttClient *)Arg)->DataCallback)(((MqttClient *)Arg)->LastReceivedTopic, strlen(((MqttClient *)Arg)->LastReceivedTopic), TempData, Len); // Passing the data back
    }
    else
    {
        printf("Max payload exceeded"); /// TODO: Implement multiple package handling
    }
}

void MqttClient::mqttPublish(const char *PubTopic, const char *PubData, uint8_t QoS, bool Retain)
{
    printf("Publishing data to %s ...", PubTopic);
    err_t err = mqtt_publish(Client, PubTopic, PubData, strlen(PubData), QoS, Retain, mqttPublish_Callback, &PubTopic);
    if (err != ERR_OK)
    {
        printf("Publish err: %d\n", err);
    }
}

void MqttClient::mqttPublish_Callback(void *Arg, err_t Result)
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