//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing MQTT sending and receiving

//Libraries
#include "ELClient.h"
#include "ELClientCmd.h"
#include "ELClientMqtt.h"

//Global constants
const char* MqttROOT = "/growboxguy@gmail.com/";
const char* MqttPUBLISH = "Gbox420";
const char* MqttInternalFan = "InternalFan"; //int, 3 options: 0-Off,1-Low,2-High
const char* MqttBrightness = "Brightness";  //int
const char* MqttTimerEnabled = "TimerEnabled"; //bool
const char* MqttAeroPressureHigh = "AeroPressureHigh"; //float

//Global variables
bool MqttAlive = false;
long LastHeartBeat;
char MqttPath[64];
int count = 0;
uint32_t last;

//Component initialization
ELClient ESPLink(&Serial3);
ELClientCmd EspCmd(&ESPLink);
ELClientMqtt Mqtt(&ESPLink);

void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial.print(F("WebServer starting.."));
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
  Mqtt.lwt("/lwt", "offline", 0, 0); //declares what message should be sent on it's behalf by the broker, after Gbox420 has gone offline.
  Mqtt.setup();
  Serial.println(F("MQTT ready"));
}

void loop() {
  ESPLink.Process();
  if ( count == 0 || (millis()-last) > 60000) {
    mqttPublish();
  }
}

void mqttPublish(){
  Serial.print(F("Reporting to MQTT: "));
    Serial.println(count);
    char message[18];
    itoa(count++, message, 10);
    memset(&MqttPath[0], 0, sizeof(MqttPath)); //reset variable
    strcat(MqttPath,MqttROOT);
    strcat(MqttPath,MqttPUBLISH);
    Mqtt.publish(MqttPath, message);    
    last = millis();
}
  
void mqttConnected(void* response) {
  memset(&MqttPath[0], 0, sizeof(MqttPath)); //reset variable
  strcat(MqttPath,MqttROOT);
  strcat(MqttPath,"#"); //subscribe to all subtopics of MQTT Root
  Mqtt.subscribe(MqttPath);
  Serial.println(F("MQTT connected!"));
}

void mqttReceived(void* response) {
  ELClientResponse *res = (ELClientResponse *)response;
  char topic[64];
  char data[16]; 
  ((*res).popString()).toCharArray(topic, 64);
  ((*res).popString()).toCharArray(data, 16);

  Serial.print(F("Received: "));Serial.print(topic);Serial.print(F(" - "));Serial.println(data);
  
  if(strstr(topic,MqttPUBLISH)!=NULL) {MqttHeartBeat(); } //Subscribed to own MQTT feed: Confirming connection is still alive
  else if(strstr(topic,MqttBrightness)!=NULL) { setBrightness(atoi(data)); }
  else if(strstr(topic,MqttTimerEnabled)!=NULL) {setTimerOnOff(atoi(data));} //bool 
  else if(strstr(topic,MqttInternalFan)!=NULL) {if(strcmp(data,"2")==0)internalFanHigh(); else if(strcmp(data,"1")==0)internalFanLow(); else if(strcmp(data,"0")==0)internalFanOff(); }
  else if(strstr(topic,MqttAeroPressureHigh)!=NULL) {setAeroPressureHigh(atof(data));} 
}

void mqttPublished(void* response) {
  Serial.println(F("MQTT published"));
}

void mqttDisconnected(void* response) {
  Serial.println(F("MQTT disconnected"));
}

void MqttHeartBeat(){
  Serial.println(F("MQTT heartbeat received"));
}

//Fake methods getting triggered by MQTT messages

void setAeroPressureHigh(float PressureHigh){
  Serial.print(F("Max pressure: "));Serial.println(PressureHigh);
}

void setBrightness(int brigtness){
  Serial.print(F("Brightness: "));Serial.println(brigtness);
}

void setTimerOnOff(bool isTimerEnabled)
{
if (isTimerEnabled) Serial.println(F("Timer enabled"));
else Serial.println(F("Timer disabled"));  
}

void turnLightON(){
  Serial.println(F("Fake light turned ON"));
}

void turnLightOFF(){
  Serial.println(F("Fake light turned OFF"));
}

void internalFanHigh(){
  Serial.println(F("Fake Internal fan High"));
}

void internalFanLow(){
  Serial.println(F("Fake Internal fan Low"));
}

void internalFanOff(){
  Serial.println(F("Fake Internal fan OFF"));
}
