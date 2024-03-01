/*
 * mqttApp.h
 *
 *  Created on: 23. 10. 2013
 *      Author: hp
 */

#ifndef MQTTAPP_H_
#define MQTTAPP_H_

void mqttAppInit();
void mqttAppConnect();
void mqttAppSend();
void mqttAppHandle();
void mqttAppDisconnect();
void mqttAppPublish(char *topic, char *data);

#endif /* MQTTAPP_H_ */
