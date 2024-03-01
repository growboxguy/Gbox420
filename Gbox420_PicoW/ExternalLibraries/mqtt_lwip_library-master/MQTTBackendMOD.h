/*! \file
 *  \brief     Based on Hubmartin's MQTT LWIP library: https://github.com/hubmartin/mqtt_lwip_library/tree/master
 *  \details
 *  \attention
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/raspberry-pi-pico-w
 *  \version   4.20
 */
#pragma once

#include <stdint.h>
#include "lwip/opt.h"
#include "lwip/ip_addr.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
// #include "netif/loopif.h"

#define MQTTCONNECT 1 << 4
#define MQTTPUBLISH 3 << 4
#define MQTTSUBSCRIBE 8 << 4
#define MAX_PACKET_SIZE 128
#define KEEPALIVE 5000
#define MQTT_MSGT_PINGREQ (12 << 4)
#define MQTT_MSGT_PINGRESP (13 << 4)
#define MQTT_MSGT_PUBLISH (3 << 4)
#define MQTT_MSGT_CONACK (2 << 4)
#define MQTT_PINGREQ_HEADER (MQTT_MSGT_PINGREQ)

class MQTTBackend : virtual public Common
{

typedef void (*mqttReceivedCallback)(uint8_t *topic, uint8_t topicLen, uint8_t *data, uint32_t dataLen);
typedef struct MqttFixedHeader
{
	uint8_t header;
	uint8_t remainingLength;
} MqttFixedHeader;


public:
    MQTTBackend(const char *Name, Module *Parent, Settings::MQTTSettings *DefaultSettings, mqttReceivedCallback fn);
	void mqttInit(Mqtt *mqtt, struct ip_addr serverIp, int port, mqttReceivedCallback fn, char *devId);
	uint8_t mqttConnect();
	uint8_t mqttPublish(char *pub_topic, char *msg);
	uint8_t mqttDisconnect();
	uint8_t mqttSubscribe(, char *topic);
	uint8_t mqttLive();
	void mqttDisconnectForced();

private:
protected:
	ip4_addr server;
	uint16_t port;
	uint8_t connected;
	uint8_t autoConnect;
	struct tcp_pcb *pcb;
	uint32_t lastActivity;
	mqttReceivedCallback mqttReceivedCallbackCallback;
	char *deviceId;
	uint8_t pollAbortCounter;
};
