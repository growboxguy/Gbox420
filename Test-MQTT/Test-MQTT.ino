#include <ELClient.h>
#include <ELClientCmd.h>
#include <ELClientMqtt.h>

ELClient esp(&Serial3);
ELClientCmd cmd(&esp);
ELClientMqtt mqtt(&esp);

void mqttConnected(void* response) {
  Serial.println("MQTT connected!");
  mqtt.subscribe("/growboxguy@gmail.com/Controls");
}

void mqttDisconnected(void* response) {
  Serial.println("MQTT disconnected");
}

void mqttData(void* response) {
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

void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial.println("EL-Client starting!");


  bool ok;
  do {
    ok = esp.Sync();      // sync up with esp-link, blocks for up to 2 seconds
    if (!ok) Serial.println("EL-Client sync failed!");
  } while(!ok);
  Serial.println("EL-Client synced!");

   mqtt.connectedCb.attach(mqttConnected);
  mqtt.disconnectedCb.attach(mqttDisconnected);
  mqtt.publishedCb.attach(mqttPublished);
  mqtt.dataCb.attach(mqttData);
  mqtt.setup();

  //Serial.println("ARDUINO: setup mqtt lwt");
  //mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

  Serial.println("EL-MQTT ready");
}

static int count = 0;
static uint32_t last;

void loop() {
  esp.Process();

  if ( count == 0 || (millis()-last) > 60000) {
    Serial.println("publishing");
    char buf[12];

    itoa(count++, buf, 10);
    mqtt.publish("/growboxguy@gmail.com/", buf);

    uint32_t t = cmd.GetTime();
    Serial.print("Time: "); Serial.println(t);

    last = millis();
  }
}
