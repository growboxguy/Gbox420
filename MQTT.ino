//Global variables
char MqttPath[64];  //buffer

//Global constants
const char* MqttROOT = "/growboxguy@gmail.com/";  //all mqtt messages have this root topic in front of them
const char* MqttPUBLISH = "Gbox420"; //MQTT logs will be sent to this topic.
const char* MqttLwtTopic = "LWT";  //When the connection is lost the MQTT broker will publish a final message to this topic
const char* MqttLwtMessage = "Gbox420 Offline"; //this is the message subscribers will get under the topic specified by MqttLwtTopic variable when the box goes offline

const char* MqttLights = "Lights";  
const char* MqttBrightness = "Brightness";
const char* MqttDisplayBrightness = "DisplayBrightness";
const char* MqttDisplayValue = "DisplayValue";
const char* MqttTimerEnabled = "TimerEnabled";
const char* MqttLightsOnHour = "LightsOnHour";
const char* MqttLightsOnMinute = "LightsOnMinute";
const char* MqttLightsOffHour = "LightsOffHour";
const char* MqttLightsOffMinute = "LightsOffMinute";
const char* MqttAeroInterval = "AeroInterval";
const char* MqttAeroDuration = "AeroDuration";
const char* MqttAeroSprayEnabled = "AeroSprayEnabled";
const char* MqttLightCalibrate = "LightCalibrate";
const char* MqttEe = "Ee";
const char* MqttInternalFan = "InternalFan";
const char* MqttExhaustFan = "ExhaustFan";
const char* MqttSoundEnabled = "Sound";
const char* MqttDebugEnabled = "Debug";
const char* MqttGoogleSheets = "GoogleSheets";
const char* MqttSaveSettings = "SaveSettings";
const char* MqttAeroSprayNow = "AeroSprayNow";
const char* MqttAeroSprayOff = "AeroSprayOff";
const char* MqttPumpDisable = "PumpDisable";
const char* MqttPressureCalibrate = "PressureCalibrate";
//const char* Mqtt = "";

void setupMqtt(){
  Mqtt.connectedCb.attach(mqttConnected);
  Mqtt.disconnectedCb.attach(mqttDisconnected);
  Mqtt.publishedCb.attach(mqttPublished);
  Mqtt.dataCb.attach(mqttReceived);
  memset(&MqttPath[0], 0, sizeof(MqttPath)); //reset variable to store the Publish to path
  strcat(MqttPath,MqttROOT);
  strcat(MqttPath,MqttLwtTopic);
  Mqtt.lwt(MqttPath, MqttLwtMessage, 0, 1); //(topic,message,qos,retain) declares what message should be sent on it's behalf by the broker after Gbox420 has gone offline.
  Mqtt.setup();
}

void mqttReceived(void* response) {
  ELClientResponse *res = (ELClientResponse *)response;
  char topic[64];
  char data[16]; 
  ((*res).popString()).toCharArray(topic, 64);
  ((*res).popString()).toCharArray(data, 16);

  logToSerials(F("Received: "),false);logToSerials(topic,false);logToSerials(F(" - "),false);logToSerials(data,true);
  if(strstr(topic,MqttLights)!=NULL) { if(strcmp(data,"1")==0)turnLightON(true); else if(strcmp(data,"0")==0)turnLightOFF(true); }
  else if(strstr(topic,MqttBrightness)!=NULL) { setBrightness(atoi(data),true); }
  else if(strstr(topic,MqttDisplayBrightness)!=NULL) {setDigitDisplayBacklight(atoi(data));}
  else if(strstr(topic,MqttDisplayValue)!=NULL) {setDigitDisplayValue(atoi(data));}
  else if(strstr(topic,MqttTimerEnabled)!=NULL) {setTimerOnOff(atoi(data));} //bool 
  else if(strstr(topic,MqttLightsOnHour)!=NULL) {setLightsOnHour(atoi(data));}
  else if(strstr(topic,MqttLightsOnMinute)!=NULL) {setLightsOnMinute(atoi(data));}
  else if(strstr(topic,MqttLightsOffHour)!=NULL) {setLightsOffHour(atoi(data));}
  else if(strstr(topic,MqttLightsOffMinute)!=NULL) {setLightsOffMinute(atoi(data));}       
  else if(strstr(topic,MqttAeroInterval)!=NULL) { setAeroInterval(atoi(data));}
  else if(strstr(topic,MqttAeroDuration)!=NULL) {setAeroDuration(atoi(data));} 
  else if(strstr(topic,MqttAeroSprayEnabled)!=NULL) {setAeroSprayOnOff(atoi(data));}
  else if(strstr(topic,MqttLightCalibrate)!=NULL) {triggerCalibrateLights();}
  else if(strstr(topic,MqttEe)!=NULL) { playEE(); }
  else if(strstr(topic,MqttInternalFan)!=NULL) {if(strcmp(data,"2")==0)internalFanHigh(); else if(strcmp(data,"1")==0)internalFanLow(); else if(strcmp(data,"0")==0)internalFanOff(); }
  else if(strstr(topic,MqttExhaustFan)!=NULL) {if(strcmp(data,"2")==0)exhaustFanHigh(); else if(strcmp(data,"1")==0)exhaustFanLow(); else if(strcmp(data,"0")==0)exhaustFanOff(); }
  else if(strstr(topic,MqttSoundEnabled)!=NULL) {setSoundOnOff(atoi(data));}
  else if(strstr(topic,MqttDebugEnabled)!=NULL) {setDebugOnOff(atoi(data));}
  else if(strstr(topic,MqttGoogleSheets)!=NULL) { ReportToGoogleSheets(true);} 
  else if(strstr(topic,MqttSaveSettings)!=NULL) { saveSettings(true);}
  else if(strstr(topic,MqttAeroSprayNow)!=NULL) { aeroSprayNow(true);}
  else if(strstr(topic,MqttAeroSprayOff)!=NULL) { aeroSprayOff();}  
  else if(strstr(topic,MqttPumpDisable)!=NULL) { aeroPumpDisable();}  
  else if(strstr(topic,MqttPressureCalibrate)!=NULL) { calibratePressureSensor();}

  mqttPublush(false); //send out a fresh report
  
}

void mqttPublush(bool AddToLog){ //publish readings in JSON format
  if(AddToLog)addToLog(F("Reporting to MQTT"));
  logToJSON(false,false); //fills Message global variable
  memset(&MqttPath[0], 0, sizeof(MqttPath)); //reset variable
  strcat(MqttPath,MqttROOT);
  strcat(MqttPath,MqttPUBLISH);
  logToSerials(F("Reporting to MQTT: "),false);logToSerials(MqttPath,false); logToSerials(F(" - "),false); logToSerials(Message,true);
  Mqtt.publish(MqttPath, Message,0,1); //(topic,message,qos,retain)
}

void mqttConnected(void* response) {
  memset(&MqttPath[0], 0, sizeof(MqttPath)); //reset variable
  strcat(MqttPath,MqttROOT);
  strcat_P(MqttPath,(PGM_P)F("#"));
  Mqtt.subscribe(MqttPath);
  logToSerials(F("MQTT connected!"),true);
}

void mqttDisconnected(void* response) {
  logToSerials(F("MQTT disconnected"),true);
}

void mqttPublished(void* response) {
  logToSerials(F("MQTT published"),true);
}
