#include "MqttClient.h"

MqttClient::MqttClient(Module *Parent, Settings::MqttClientSettings *DefaultSettings, CallbackType_mqtt DataCallback) : Common(DefaultSettings->Name)
{
    this->DataCallback = DataCallback;
    MqttServerDNS = DefaultSettings->MqttServerDNS;
    MqttServerPort = &DefaultSettings->MqttServerPort;
    PubTopic = DefaultSettings->PubTopic;
    SubTopic = DefaultSettings->SubTopic;
    PublishRetain = &DefaultSettings->PublishRetain;
    QoS = &DefaultSettings->QoS;
    Parent->addToRefreshQueue_1min(this);
    Parent->addToReportQueue(this);

    Client = mqtt_client_new(); // Allocate memory for the Stuct storing the MQTT client, store the pointer to it
    ClientInfo = new mqtt_connect_client_info_t();
    ClientInfo->client_id = DefaultSettings->ClientID;
    ClientInfo->client_user = DefaultSettings->MqttServerUser;
    ClientInfo->client_pass = DefaultSettings->MqttServerPassword;
    ClientInfo->keep_alive = DefaultSettings->KeepAliveSeconds;
    ClientInfo->will_topic = DefaultSettings->LwtTopic;
    ClientInfo->will_msg = DefaultSettings->LwtMessage;
    ClientInfo->will_qos = DefaultSettings->QoS;
    ClientInfo->will_retain = DefaultSettings->LwtRetain;

    mqttConnectTrigger(true);                                  // Connect to the MQTT server
    absolute_time_t NextRefresh = make_timeout_time_ms(10000); // 10sec from now
    while (InProgress_ConnectAndSubscribe)                     // Waiting for the MQTT connection to establish
    {
        if (get_absolute_time() > NextRefresh) // 10sec timeout
        {
            printf("MQTT connect timeout\n");
            break; ///< Stop waiting for the callback, processing will continue. Once a Connected callback arrives the result will be printed to stdout
        }
        sleep_ms(500);
    }
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void MqttClient::report(bool FriendlyFormat)
{
    Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
    strcat(LongMessage, "\"S\":\"");
    strcat(LongMessage, mqttIsConnectedText(FriendlyFormat));
    strcat(LongMessage, "\"}"); ///< closing the curly bracket at the end of the JSON
}

void MqttClient::run1min()
{
    Common::run1min();
    mqttCheck();
}

/// @brief Call to check mqtt status and reconnect to server if needed.
void MqttClient::mqttCheck()
{
    if (!mqttIsConnected())
    {
        mqttConnectTrigger(false);
    }
}

void MqttClient::mqttDNSResolvedCallback(ip_addr_t *ServerIP)
{
    MqttServerAddress = *ServerIP;
    mqttConnect();
}

/// @brief Connect to MQTT server, if MqttServerDNS is specified an extra DNS lookup is also needed
/// @param WaitForIP true: Wait for the DNS lookup result before attempting to connect (Cannot be called from a callback like run1sec/run5sec/run1min/report). false: Start DNS lookup in the background, and in the meantime attempt to connect using MqttServerAddress. The next time mqttConnectTrigger() is called the DNS lookup result should be already cached
void MqttClient::mqttConnectTrigger(bool WaitForIP)
{
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP) // Returns the status of the WiFi link: CYW43_LINK_DOWN(0)-link is down,CYW43_LINK_JOIN(1)-Connected to WiFi,CYW43_LINK_NOIP(2)-Connected to WiFi, but no IP address,CYW43_LINK_UP  (3)-Connect to WiFi with an IP address,CYW43_LINK_FAIL(-1)-Connection failed,CYW43_LINK_NONET(-2)-No matching SSID found (could be out of range, or down),CYW43_LINK_BADAUTH(-3)-Authentication failure
    {
        InProgress_ConnectAndSubscribe = true;
        ip4addr_aton(MqttServerIP, &MqttServerAddress); // If MqttServerDNS is defined and the DNS lookup is successful this will be overwritten
        if (MqttServerDNS[0] != '\0')                   // If an MQTT server DNS name is specified -> Look up the IP
        {
            if (WaitForIP)
            {
                if (DnsLookup(MqttServerDNS, &MqttServerAddress)) // Resolve DNS name, can take up to one sec
                {
                    mqttConnect();
                }
            }
            else
            {
                std::function<void(ip_addr_t *)> callbackFunctionPtr = std::bind(&MqttClient::mqttDNSResolvedCallback, this, std::placeholders::_1);
                DnsLookup_Async(MqttServerDNS, &MqttServerAddress, callbackFunctionPtr); // Start resolving DNS name, if cached result is available it is instantly returned to MqttServerAddress
            }
        }
    }
    else
    {
        printf("   MQTT failed, no WiFi\n");
    }
}

void MqttClient::mqttConnect()
{
    printf("Connecting to MQTT server...");
    cyw43_arch_lwip_begin();
    err_t err = mqtt_client_connect(Client, &MqttServerAddress, *MqttServerPort, mqttConnect_Callback, this, ClientInfo);
    cyw43_arch_lwip_end();
    if (err != ERR_OK)
    {
        printf("error: %d\n", err);
    }
}

void MqttClient::mqttConnect_Callback(mqtt_client_t *Client, void *Arg, mqtt_connection_status_t Status)
{
    if (Status == MQTT_CONNECT_ACCEPTED)
    {
        printf("MQTT Connected\n");
        ((MqttClient *)Arg)->mqttSubscribe(); // Once connected subscribe to incoming messages
    }
    else
    {
        printf("MQTT Disconnected, reason: %d\n", Status); // 0: Accepted, 1:Protocol version refused, 2:Identifier refused, 3:Server refused, 4:Credentials refused, 5:Not authorized, 256:MQTT Disconnect, 257:Timeout
        ((MqttClient *)Arg)->InProgress_ConnectAndSubscribe = false;
    }
}

void MqttClient::mqttDisconnect()
{
    mqtt_disconnect(Client);
}

bool MqttClient::mqttIsConnected()
{
    if (mqtt_client_is_connected(Client) == 1)
        return true;
    else
        return false;
}

char *MqttClient::mqttIsConnectedText(bool FriendlyFormat)
{
    if (FriendlyFormat)
    {
        return toText_connectedStatus(mqttIsConnected());
    }
    else
    {
        return toText(mqttIsConnected());
    }
}

void MqttClient::mqttSubscribe()
{
    InProgress_ConnectAndSubscribe = true;
    mqtt_set_inpub_callback(Client, mqttIncomingTopic_Callback, mqttIncomingData_Callback, this); // Set callback functions
    err_t err = mqtt_sub_unsub(Client, SubTopic, *QoS, mqttSubscribe_Callback, this, true);       // Initiate subscription
    if (err != ERR_OK)
    {
        printf("Error subscribing to %s - Error: %s\n", SubTopic, err);
    }
}

void MqttClient::mqttSubscribe_Callback(void *Arg, err_t Result)
{
    if (Result == 0)
    {
        printf("Subscribed to %s\n", ((MqttClient *)Arg)->SubTopic);
    }
    else
    {
        printf("Failed to subscribe to %s - Error: %s\n", ((MqttClient *)Arg)->SubTopic, Result);
    }
    ((MqttClient *)Arg)->InProgress_ConnectAndSubscribe = false;
}

void MqttClient::mqttUnsubscribe()
{
    printf("Unsubscribing from %s...", SubTopic);
    err_t err = mqtt_sub_unsub(Client, SubTopic, *QoS, mqttUnsubscribe_Callback, this, false);
    if (err != ERR_OK)
    {
        printf("error: %d\n", err);
    }
}

void MqttClient::mqttUnsubscribe_Callback(void *Arg, err_t Result)
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

// When an MQTT message arrives first the topic of the message is passed in this callback
void MqttClient::mqttIncomingTopic_Callback(void *Arg, const char *Topic, uint32_t Tot_len)
{
    strcpy(((MqttClient *)Arg)->LastReceivedTopic, Topic);
    // printf("MQTT incoming topic: %s\n", ((MqttClient *)Arg)->LastReceivedTopic);
}

/// @brief Callback when MQTT data arrives on a Subscribed topic
/// @param Arg Contains a pointer to an MqttClient object
/// @param Data Pointer to the memory address of the first character in the received data
/// @param DataLength Number of characters received (Not null terminated!)
/// @param Flags 0 - More data packages to follow, 1 - Last data package
void MqttClient::mqttIncomingData_Callback(void *Arg, const uint8_t *Data, uint16_t DataLength, uint8_t Flags)
{
    if (Flags == 1 && DataLength < MaxLongTextLength) // Last fragment of payload received (or the whole payload fits receive buffer)
    {
        char TopicReceived[MaxShotTextLength] = {};                                  // Initialize null terminated Topic storage
        size_t SubTopicLength = strlen(((MqttClient *)Arg)->SubTopic) - 1;           // Get length of the subscribed topic, minus the # sign (# is wildcard for any subtopic)
        size_t ReceivedTopicLength = strlen(((MqttClient *)Arg)->LastReceivedTopic); // Get length of the received topic
        if (SubTopicLength < ReceivedTopicLength)
        {
            memcpy(TopicReceived, ((MqttClient *)Arg)->LastReceivedTopic + SubTopicLength, ReceivedTopicLength - SubTopicLength + 1); // Chop the pre-known part of the Topic. Example: Subscribed topic: Gbox420CMD/#  Received: Gbox420CMD/Light1_ON --> Light1_ON
        }
        char DataReceived[MaxLongTextLength] = {}; // Initialize null terminated Data storage
        memcpy(DataReceived, Data, DataLength);    // Copy data to null terminated buffer

        /*
        printf("TopicMQTT: %s\n", ((MqttClient *)Arg)->LastReceivedTopic);
        printf("TopicMQTTShort: %s\n",TopicReceived);
        printf("DataMQTT: %s\n", DataReceived);
        */
        ((MqttClient *)Arg)->DataCallback(TopicReceived, DataReceived); // Passing the data back by calling DataCallback. Passing parameters: Topic,Data and DataLength
    }
    else
    {
        printf("MQTT data exceeded buffer"); /// TODO: Implement multiple package handling? Not really needed, since incoming data is small
    }
}

void MqttClient::mqttPublish(char *PubTopic, char *PubData)
{
    err_t err = mqtt_publish(Client, PubTopic, PubData, strlen(PubData), ClientInfo->will_qos, *PublishRetain, mqttPublish_Callback, PubTopic);
    if (err != ERR_OK)
    {
        printf("  MQTT publish error: %d\n", err); // Failed to send out publish request
    }
}

void MqttClient::mqttPublish_Callback(void *Arg, err_t Result)
{
    if (Result != ERR_OK)
    {
        printf("  MQTT publish to %s failed: %d\n", Arg, Result); // Server rejected publish request
    }
    /*
    else
    {
        printf("  MQTT published to %s\n", Arg);
    }
    */
}