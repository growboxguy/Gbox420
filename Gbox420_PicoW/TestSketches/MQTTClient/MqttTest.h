// #include and #define commands moved to the MqttTest.cpp for better overview

int main();                                                                 ///< Test logic: Initialize WiFi -> Resolve MQTT server IP -> Connect to MQTT server -> Subscribe to a test topic (defined by SubTopic) -> Publish a fake JSON to an MQTT topic (defined by PubTopic)
void initializeWiFi();                                                      ///< Connect to the WiFi network
void mqttReceivedData(const u8_t *Data, u16_t Len);                         ///< Prints MQTT data received from the Subscribed topic
char *intToChar(int Number);                                                ///< Used when generating the fake JSON data to Publish
void mqttIpFound(const char *Hostname, const ip_addr_t *Ipaddr, void *Arg); // Called When the IP address of the MQTT server is found
