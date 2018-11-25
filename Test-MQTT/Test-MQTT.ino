//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing MQTT sending and receiving

//Libraries
#include <ELClient.h>
#include <ELClientCmd.h>
#include <ELClientMqtt.h>

//Global constants
const char* MqttPublish = "/growboxguy@gmail.com/Gbox420/";
const char* MqttSubscribe = "/growboxguy@gmail.com/Gbox420Control/";

//Component initialization
ELClient ESPLink(&Serial3);
ELClientCmd EspCmd(&ESPLink);
ELClientMqtt Mqtt(&ESPLink);

void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial.print("WebServer starting..");
  while(!ESPLink.Sync())  {
    Serial.print(".");
    Serial3.print(".");
    delay(500);
    };
  Serial.println("ready");
    
  Mqtt.connectedCb.attach(mqttConnected);
  Mqtt.disconnectedCb.attach(mqttDisconnected);
  Mqtt.publishedCb.attach(mqttPublished);
  Mqtt.dataCb.attach(mqttReceived);
  Mqtt.setup();
  Mqtt.lwt("/lwt", "offline", 0, 0); //declares what message should be sent on it's behalf by the broker, after Gbox420 has gone offline.
  Serial.println("MQTT ready");
}

static int count = 0;
static uint32_t last;

void loop() {
  ESPLink.Process();
  if ( count == 0 || (millis()-last) > 10000) {
    Serial.print("Publishing: ");
    Serial.println(count);
    char buf[18];
    itoa(count++, buf, 10);
    Mqtt.publish(MqttPublish, buf);    
    last = millis();
  }
}

void mqttConnected(void* response) {
  Serial.println("MQTT connected!");
  Mqtt.subscribe(MqttSubscribe);
}

void mqttDisconnected(void* response) {
  Serial.println("MQTT disconnected");
}

void mqttReceived(void* response) {
  ELClientResponse *res = (ELClientResponse *)response;

  Serial.print("Received: topic=");
  String topic = res->popString();
  Serial.println(topic);

  Serial.print("data=");
  String data = res->popString();
  Serial.println(data);
}

void mqttPublished(void* response) {
  Serial.println("MQTT published");
}


