#include "MQTT.h"

MQTT::MQTT(const char *Name, Module *Parent, Settings::MQTTSettings *DefaultSettings, mqttReceivedCallback fn) : Common(Name)
{
  this->Parent = Parent;
  Enabled = &DefaultSettings->Enabled;
  Address = &DefaultSettings->Address;
  Port = &DefaultSettings->Port;
  ReportingFrequency = &DefaultSettings->ReportingFrequency;
  PubTopic = &DefaultSettings->PubTopic;
  SubTopic = &DefaultSettings->SubTopic;
  LwtTopic = &DefaultSettings->LwtTopic;
  LwtMessage = &DefaultSettings->LwtMessage;
  //setupMqtt();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue(this);
  printf("   MQTT ready\n");
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void MQTT::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat(LongMessage, "\"S\":\"");
  strcat(LongMessage, getEnabledStateText(FriendlyFormat));
  strcat(LongMessage, "\"}"); ///< closing the curly bracket at the end of the JSON
}

void MQTT::refresh()
{
  if (*Enabled && RefreshCounter % (*ReportingFrequency) == 0)
  {
    Common::refresh();
    /*
        if (MQTTReportRequested)
        {
          MQTTReportRequested = false;
          reportToMQTTTrigger(true);
        }
        */
  }
}

/*
  \brief Sets up the MQTT relay: Configures callbacks for MQTT events and sets the Last Will and Testament in case the ESP-link goes offline
*/

/*
void MQTT::setupMqtt()
{

  ip_addr_t server;
  dns_gethostbyname(NTP_SERVER, &server, setupMqtt_dnsFound, NULL);


    struct ip_addr serverIp;
    IP4_ADDR(&serverIp, 192,168,100,140);

    mqttInit(&mqtt, serverIp, 1883, &mqttAppMsgReceived, "Stellaris");
    */

  /*
    MqttAPI.connectedCb.attach(mqttConnected);
    MqttAPI.disconnectedCb.attach(mqttDisconnected);
    MqttAPI.publishedCb.attach(mqttPublished);
    MqttAPI.dataCb.attach(mqttReceived);

    memset(&ShortMessage[0], 0, MaxShotTextLength); //reset variable to store the Publish to path
    strcat(ShortMessage, ModuleSettings->LwtTopic);
    MqttAPI.lwt(ShortMessage, ModuleSettings->LwtMessage, 0, 1); //(topic,message,qos,retain) declares what message should be sent on it's behalf by the broker after Gbox420 has gone offline.
    MqttAPI.setup();
    
}
*/

/*
// Called by dns_gethostbyname() when the IP address of MQTT server is found
void MQTT::setupMqtt_dnsFound(const char *hostname, const ip_addr_t *ipaddr, void *arg)
{
  if (ipaddr)
  {
    printf("MQTT address %s\n", ipaddr_ntoa(ipaddr));
    mqttConnect(ipaddr);
  }
  else
  {
    printf("MQTT DNS request failed\n");
  }
}

*/



/**
  \brief Called when an MQTT command is received. Extracts the command from the MQTT topic and gets the data passed along the command
  \param response The MQTT message object passed by ESP-link (type: ELClientResponse)
*/
void MQTT::mqttReceived(void *response)
{
  // static uint8_t SubTopicLength = strlen(ModuleSettings->SubTopic) - 1; //Get length of the command topic
  static char command[MaxShotTextLength];
  static char data[MaxShotTextLength];
  // ELClientResponse *res = (ELClientResponse *)response;
  // String mqttTopic = (*res).popString();
  // String mqttData = (*res).popString();
  printf("MQTT\n");
  //  printf(" %s",&mqttTopic);
  // mqttTopic.remove(0, SubTopicLength); //Cut the known command topic from the arrived topic
  // mqttTopic.toCharArray(command, MaxShotTextLength);
  // mqttData.toCharArray(data, MaxShotTextLength);
  // Hempy_Standalone1->commandEventTrigger(command, data);
  // Hempy_Standalone1->reportToMQTTTrigger(true); // send out a fresh report
}

void MQTT::setMQTTOnOff(bool State)
{
  *Enabled = State;
  Parent->addToLog(getName(getEnabledStateText(true)));
}

bool MQTT::getEnabledState()
{
  return *Enabled;
}

char *MQTT::getEnabledStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_enabledDisabled(*Enabled);
  }
  else
  {
    return toText(*Enabled);
  }
}
/*
void MQTT::setMQTTReportingOnOff(bool State)
{
  *ReportToMQTT = State;
  if (*ReportToMQTT)
  {
    addToLog("MQTT ON");
  }
  else
  {
    addToLog("MQTT OFF");
  }
  getSoundObject()->playOnOffSound(*ReportToMQTT);
}

void MQTT::setMQTTReportingFrequency(uint16_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog("MQTT freqency updated");
}

void MQTT::setMqttPublishTopic(const char *Topic)
{
  strncpy(ModuleSettings->PubTopic, Topic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog("MQTT publish updated");
}

void MQTT::setMqttSubscribeTopic(const char *Topic)
{
  strncpy(ModuleSettings->SubTopic, Topic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog("MQTT subscribe updated");
}

void MQTT::setLwtTopic(const char *LWTTopic)
{
  strncpy(ModuleSettings->LwtTopic, LWTTopic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog("LWT topic updated");
}

void MQTT::setLwtMessage(const char *LWTMessage)
{
  strncpy(ModuleSettings->LwtMessage, LWTMessage, MaxWordLength);
  getSoundObject()->playOnSound();
  addToLog("LWT message updated");
}
*/