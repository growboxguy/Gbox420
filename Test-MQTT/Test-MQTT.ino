//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing MQTT sending and receiving

//Libraries
#include "ELClient.h"
#include "ELClientMqtt.h"

//Global constants
const char* MqttROOT = "/growboxguy@gmail.com/"; //Root of every MQTT message
const char* MqttPUBLISH = "Gbox420";  //Readings get published under this topic
const char* MqttInternalFan = "InternalFan"; //MQTT command the code responds to 
const char* MqttBrightness = "Brightness";  //MQTT command the code responds to
const char* MqttLightOn = "LightOn"; //MQTT command the code responds to
const char* MqttAeroPressureHigh = "AeroPressureHigh"; //MQTT command the code responds to
const char* MqttString = "MqttString"; //char
    
//Global variables
bool MqttAlive = false;
unsigned long LastHeartBeat;
unsigned long LastPublish;
char MqttPath[64];
int PublishedCounter = 0;
//fake variables to report and update
int InternalFan = 0;
int LightBrightness = 50;
bool LightOn=false;
float AeroPressureHigh = 7.1;
char TestString[30] = "Gbox420";

//Component initialization
ELClient ESPLink(&Serial3);
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
  if ( PublishedCounter == 0 || (millis()-LastPublish) > 10000) {
    PublishedCounter++;
    mqttPublish();
    LastPublish = millis(); 
  }
}

void mqttPublish(){ 
    char WebMessage[512];   //buffer for MQTT API messages (severly oversized)
    memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
    strcat(WebMessage,"{\"Counter\":\"");  strcat(WebMessage,intToChar(PublishedCounter));
    strcat(WebMessage,"\",\"InternalFan\":\""); strcat(WebMessage,intToChar(InternalFan));
    strcat(WebMessage,"\",\"Brightness\":\"");  strcat(WebMessage,intToChar(LightBrightness));
    strcat(WebMessage,"\",\"LightOn\":\"");  strcat(WebMessage,intToChar(LightOn)); //bool is int (0 or 1)
    strcat(WebMessage,"\",\"AeroPressureHigh\":\"");  strcat(WebMessage,floatToChar(AeroPressureHigh));
    strcat(WebMessage,"\",\"MqttString\":\"");  strcat(WebMessage,TestString);  
    strcat(WebMessage,"\"}");  //closing the JSON formatted 
      
    memset(&MqttPath[0], 0, sizeof(MqttPath)); //reset variable to store the Publish to path
    strcat(MqttPath,MqttROOT);
    strcat(MqttPath,MqttPUBLISH);
    
    Serial.print(PublishedCounter); Serial.print(". publish to: ");Serial.print(MqttPath);Serial.print(" - ");Serial.println(WebMessage);
    Mqtt.publish(MqttPath, WebMessage);       
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

  //Checking what keyword the topic contains
  if(strstr(topic,MqttPUBLISH)!=NULL) {MqttHeartBeat(); } //Subscribed to own MQTT Publish feed: Confirming last publish was successful
  else if(strstr(topic,MqttBrightness)!=NULL) { setBrightness(atoi(data)); } //expects int parameter
  else if(strstr(topic,MqttLightOn)!=NULL) {setLightOnOff(atoi(data));} //expects bool parameter
  else if(strstr(topic,MqttInternalFan)!=NULL) {setFanSpeed(atoi(data));} //expects int parameter, 3 options: 0-Off,1-Low,2-High
  else if(strstr(topic,MqttAeroPressureHigh)!=NULL) {setAeroPressureHigh(atof(data));}  //expects float parameter
  else if(strstr(topic,MqttString)!=NULL) {setMqttSting(data);}  //expects char array parameter
}

void mqttDisconnected(void* response) {
  Serial.println(F("MQTT disconnected"));
  MqttAlive = false;
}

void MqttHeartBeat(){
  Serial.println(F("MQTT heartbeat received"));
  MqttAlive = true;
  LastHeartBeat = millis();
}

void mqttPublished(void* response) {
  Serial.println(F("MQTT published"));
}

//Methods for converting to char 
char * intToChar(int Number){
  static char ReturnChar[8] = ""; //7 digits + null terminator max
  itoa(Number, ReturnChar, 10);
  return ReturnChar;
}

char * floatToChar(float Number){  
  static char ReturnFloatChar[8] = ""; //4 digits + decimal sign + 2 decimals + null terminator
  dtostrf(Number, 4, 2, ReturnFloatChar); 
  return ReturnFloatChar;
}

//Fake methods getting triggered by MQTT messages

void setAeroPressureHigh(float PressureHigh){
  AeroPressureHigh = PressureHigh;
  Serial.print(F("Max pressure: "));Serial.println(PressureHigh);
}

void setBrightness(int brigtness){
  LightBrightness = brigtness;
  Serial.print(F("Brightness: "));Serial.println(brigtness);
}

void setLightOnOff(bool isLightOn)
{
  LightOn =  isLightOn;
  if (isLightOn) Serial.println(F("Fake light turned ON"));
  else Serial.println(F("Fake light turned OFF"));
}

void setMqttSting(char * StringToSet){
  memset(&TestString[0], 0, sizeof(TestString)); 
  strcpy(TestString, StringToSet);
  Serial.print("Word of the day is: ");Serial.println(StringToSet);
}

void setFanSpeed(int FanSpeed){
  InternalFan = FanSpeed;
  if(FanSpeed==2){
    Serial.println(F("Fake Internal fan High"));
  } 
  else if(FanSpeed==1){
    Serial.println(F("Fake Internal fan Medium"));
  } 
  else if(FanSpeed==0){
    Serial.println(F("Fake Internal fan Low"));
  }  
}
