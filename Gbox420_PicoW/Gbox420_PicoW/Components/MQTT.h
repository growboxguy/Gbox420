/*! \file
 *  \brief     MQTT handler
 *  \details
 *  \attention
 *             
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/raspberry-pi-pico-w
 *  \version   4.20
 */

#pragma once

#include "420Common.h"
#include "420Module.h"
#include "MQTTBackend.h"

class MQTT : virtual public Common
{
typedef void (*mqttReceivedCallback)(char *topic, uint8_t topicLen, char *data, uint32_t dataLen);

public:
  MQTT(const char *Name, Module *Parent, Settings::MQTTSettings *DefaultSettings,mqttReceivedCallback fn);
  void report(bool FriendlyFormat = false);
  void refresh();
  void setMQTTOnOff(bool State);
  bool getEnabledState();
  char *getEnabledStateText(bool FriendlyFormat = false);

private:
protected:
  Module *Parent;
  
  
  //void setupMqtt();
  //void setupMqtt_dnsFound(const char *hostname, const ip_addr_t *ipaddr, void *arg); //When the IP address of MQTT server is found
  //bool Connected = false;            ///< Track the connection state to the MQTT broker
  void mqttDisconnect();
  void mqttDisconnectForced();
  void mqttPublished(__attribute__((unused)) void *response);
  void mqttReceived(void *response);


  void setMQTTReportingOnOff(bool State);
  void setMQTTReportingFrequency(uint16_t Frequency);
  void setMqttPublishTopic(const char *ID);
  void setMqttSubscribeTopic(const char *ID);
  void setLwtTopic(const char *ID);
  void setLwtMessage(const char *ID);
  bool *Enabled;                         ///< MQTT ON or OFF
  char (*Address)[MaxShotTextLength];    ///< MQTT server address
  uint16_t *Port;                        ///< MQTT port
  uint16_t *ReportingFrequency;          ///< In seconds - How often report the current status
  char (*PubTopic)[MaxShotTextLength];   ///< Publish MQTT messages to this topic. Ends with a forward slash
  char (*SubTopic)[MaxShotTextLength];   ///< Subscribe to messages of this topic and all sub-topic
  char (*LwtTopic)[MaxShotTextLength];   ///< When the connection is lost the MQTT broker will publish a final message to this topic. Ends with a forward slash
  char (*LwtMessage)[MaxShotTextLength]; ///< Subscribers will get this message under the topic specified by LwtTopic when the MQTT client goes offline

};