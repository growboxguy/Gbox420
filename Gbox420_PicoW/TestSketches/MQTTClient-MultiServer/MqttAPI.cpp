#include "MqttAPI.h"

MqttAPI::MqttAPI(MQTTClientSettings *DefaultSettings, void *DataCallback)
{
    this->DataCallback = DataCallback;
    MQTTServerPort = DefaultSettings->MQTTServerPort;
    Client = mqtt_client_new();                 // Allocate memory for the Stuct storing the MQTT client, store the pointer to it
    memset(&ClientInfo, 0, sizeof(ClientInfo)); // Allocate memory for the ClientInfo
    ClientInfo.client_id = DefaultSettings->ClientID;
    ClientInfo.client_user = DefaultSettings->MQTTServerUser;
    ClientInfo.client_pass = DefaultSettings->MQTTServerPassword;
    ClientInfo.keep_alive = DefaultSettings->KeepAliveSeconds;
    ClientInfo.will_topic = DefaultSettings->LwtTopic;
    ClientInfo.will_msg = DefaultSettings->LwtMessage;
    ClientInfo.will_qos = DefaultSettings->QoS;
    ClientInfo.will_retain = DefaultSettings->LwtRetain;

    absolute_time_t LastRefresh = get_absolute_time();      // Used to track timeouts
    ip4addr_aton(DefaultSettings->MQTTServerIP, &ServerIP); // If MQTTServerDNS is defined and the DNS lookup is successful this will be overwritten
    if (DefaultSettings->MQTTServerDNS[0] != '\0')          // If an MQTT server DNS name is specified -> Look up the IP
    {
        printf("Looking up IP for %s...", DefaultSettings->MQTTServerDNS);
        err_t err = dns_gethostbyname(DefaultSettings->MQTTServerDNS, &ServerIP, mqttIpFound, this);

        if (err == ERR_OK) // DNS name found in cache
        {
            printf("Found cached address\n");
            dnsLookupInProgress = false;
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

    mqttConnect();

    LastRefresh = get_absolute_time(); // Reset the timeout counter
    while (!mqttIsConnected())         // Waiting for the MQTT connection to establish
    {
        sleep_ms(100);
        if (absolute_time_diff_us(LastRefresh, get_absolute_time()) > 10000000) // 10sec timeout
        {
            printf("MQTT connect timeout\n");
            break;
        }
    }
    if (mqttIsConnected())
        mqttSubscribe_Unsubscribe(DefaultSettings->SubTopic, DefaultSettings->QoS, true); // Subscribe to MQTT messages in the SubTopic topic.
}

void MqttAPI::mqttIpFound(const char *Hostname, const ip_addr_t *Ipaddr, void *Arg) // DNS lookup callback
{
    ((MqttAPI *)Arg)->dnsLookupInProgress = false;
    if (Ipaddr)
    {
        printf("Found address: %s\n", ipaddr_ntoa(Ipaddr));
        ((MqttAPI *)Arg)->ServerIP = *Ipaddr;
    }
    else
    {
        printf("DNS lookup failed\n");
    }
}

void MqttAPI::mqttConnect()
{
    printf("Connecting to MQTT server...");
    cyw43_arch_lwip_begin();
    err_t err = mqtt_client_connect(Client, &ServerIP, MQTTServerPort, mqttConnect_Callback, DataCallback, &ClientInfo);
    cyw43_arch_lwip_end();

    if (err != ERR_OK)
    {
        printf("error: %d\n", err);
    }
    else
    {
        printf("initiated, waiting for connected callback...");
    }
}

void MqttAPI::mqttDisconnect()
{
    mqtt_disconnect(Client);
}

void MqttAPI::mqttConnect_Callback(mqtt_client_t *Client, void *Arg, mqtt_connection_status_t Status)
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

bool MqttAPI::mqttIsConnected()
{
    if (mqtt_client_is_connected(Client) == 1)
        return true;
    else
        return false;
}

void MqttAPI::mqttSubscribe_Unsubscribe(const char *SubTopic, u8_t QoS, bool Subscribe)
{
    if (Subscribe)
    {
        printf("Subscribing to %s\n", SubTopic);
    }
    else
    {
        printf("Unsubscribing from %s\n", SubTopic);
    }
    err_t err = mqtt_sub_unsub(Client, SubTopic, QoS, mqttSubscribe_Callback, &SubTopic, Subscribe);
    if (err != ERR_OK)
    {
        printf("mqttSubscribe error: %d\n", err);
    }
}

void MqttAPI::mqttSubscribe_Callback(void *Arg, err_t Result)
{
    if (Result == 0)
    {
        printf("%s - done\n", Arg);
    }
    else
    {
        printf("%s - failed: %s\n", Arg, Result);
    }
}

void MqttAPI::mqttIncomingTopic_Callback(void *Arg, const char *Topic, u32_t Tot_len)
{
    printf("Incoming topic: %s ,total length: %u\n", Topic, (unsigned int)Tot_len);
}

void MqttAPI::mqttIncomingData_Callback(void *Arg, const u8_t *Data, u16_t Len, u8_t Flags)
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

void MqttAPI::mqttPublish(const char *PubTopic, const char *PubData, u8_t QoS, bool Retain)
{
    printf("Publishing data to %s ...", PubTopic);
    err_t err = mqtt_publish(Client, PubTopic, PubData, strlen(PubData), QoS, Retain, mqttPublish_Callback, &PubTopic);
    if (err != ERR_OK)
    {
        printf("Publish err: %d\n", err);
    }
}

void MqttAPI::mqttPublish_Callback(void *Arg, err_t Result)
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