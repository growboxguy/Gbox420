/*
 * mqttApp.c
 *
 *  Created on: 23. 10. 2013
 *      Author: hp
 */


#include "mqttApp.h"


#include "mqtt.h"

#include "utils/uartstdio.h"

Mqtt mqtt;

#include <string.h>


void mqttAppMsgReceived(Mqtt *this, uint8_t *topic, uint8_t topicLen, uint8_t *data, uint32_t dataLen)
{
	uint8_t strTopic[topicLen + 1];
	memcpy(strTopic, topic, topicLen);
	strTopic[topicLen] = '\0';

	uint8_t strData[dataLen + 1];
	memcpy(strData, data, dataLen);
	strData[dataLen] = '\0';

	UARTprintf("Topic: %s, Data: %s", strTopic, strData);

}

void mqttAppSend()
{
    uint8_t flag = mqttPublish(&mqtt, "/presence", "Hello, here is mbed!");
}

void mqttAppInit()
{
	struct ip_addr serverIp;
	IP4_ADDR(&serverIp, 192,168,100,140);

	mqttInit(&mqtt, serverIp, 1883, &mqttAppMsgReceived, "Stellaris");
}

void mqttAppConnect()
{
	uint32_t flag;

	mqtt.autoConnect = 0;

    flag = mqttConnect(&mqtt);

    //mqttSubscribe(&mqtt, "/rf/#");

    //mqttAppSend();

}

void mqttAppPublish(char *topic, char *data)
{
	mqttPublish(&mqtt, topic, data);
}

void mqttAppDisconnect()
{
	mqttDisconnectForced(&mqtt);
}


void mqttAppHandle()
{
	mqttLive(&mqtt);
}
