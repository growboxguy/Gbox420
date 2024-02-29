#pragma once

#include "420Common.h"
#include "420Module.h"
#include "hardware/pwm.h"

///< Provides MQTT support

class MQTT : virtual public Common
{
public:
  MQTT(const char *Name, Module *Parent, Settings::MQTTSettings *DefaultSettings);
  void report(bool FriendlyFormat = false);
  void checkEvents();
  void refresh();
  void playOnMQTT();
  void playOffMQTT();
  void inline pwm_calcDivTop(pwm_config *c, int frequency, int sysClock);
  void beep(int note, int duration); // Generate a PWM signal at the specified frequency for the specified duration
  void playOnOffMQTT(bool State);
  void setMQTTOnOff(bool State);
  bool getEnabledState();
  char *getEnabledStateText(bool FriendlyFormat = false);

private:
protected:
  Module *Parent;
  void setupMqtt();
  void mqttConnected(__attribute__((unused)) void *response);
  void mqttDisconnected(__attribute__((unused)) void *response);
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
  bool MqttConnected = false;            ///< Track the connection state to the MQTT broker configured on the ESP-link's REST/MQTT tab
};