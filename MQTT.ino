void setupMqtt(){
  Mqtt.connectedCb.attach(mqttConnected);
  Mqtt.disconnectedCb.attach(mqttDisconnected);
  Mqtt.publishedCb.attach(mqttPublished);
  Mqtt.dataCb.attach(mqttReceived);
  Mqtt.setup();
  Mqtt.lwt("/lwt", "offline", 0, 0); //declares what message should be sent on it's behalf by the broker, after Gbox420 has gone offline.
  Serial.println("MQTT ready");
}


void mqttConnected(void* response) {
  addToLog("MQTT connected!");
  Mqtt.subscribe(MqttSubscribe);
}

void mqttDisconnected(void* response) {
  addToLog("MQTT disconnected");
}

void mqttReceived(void* response) {
  ELClientResponse *res = (ELClientResponse *)response;

  Serial.print("MQTT: ");
  Serial.print(res->popString());
  Serial.print(" - ");
  String data = res->popString();
  Serial.println(data);
}

void mqttPublush(){
  addToLog("Reporting to MQTT");
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat(WebMessage,"BoxDate=");  strcat(WebMessage,CurrentTime);
  strcat(WebMessage,"BoxTempC=");  strcat(WebMessage,floatToChar(BoxTempC));
  strcat(WebMessage,"&BoxTempF=");  strcat(WebMessage,floatToChar(BoxTempF));
  strcat(WebMessage,"&Humidity=");  strcat(WebMessage,floatToChar(Humidity));
  strcat(WebMessage,"&Power=");  strcat(WebMessage,floatToChar(Power)); 
  strcat(WebMessage,"&Energy=");  strcat(WebMessage,floatToChar(Energy));
  strcat(WebMessage,"&Voltage=");  strcat(WebMessage,floatToChar(Voltage));
  strcat(WebMessage,"&Current=");  strcat(WebMessage,floatToChar(Current));
  strcat(WebMessage,"&PH=");  strcat(WebMessage,floatToChar(PH));
  strcat(WebMessage,"&Moisture=");  strcat(WebMessage,floatToChar(Moisture));
  strcat(WebMessage,"&isLightOn=");  strcat(WebMessage,intToChar(MySettings.isLightOn));
  strcat(WebMessage,"&Brightness=");  strcat(WebMessage,intToChar(MySettings.LightBrightness));
  strcat(WebMessage,"&LightReading=");  strcat(WebMessage,intToChar(LightReading));
  strcat(WebMessage,"&isBright=");  strcat(WebMessage,intToChar(isBright));
  strcat(WebMessage,"&Reservoir=");  strcat(WebMessage,intToChar(reservoirToPercent()));
  strcat(WebMessage,"&InternalFan="); strcat(WebMessage,fanSpeedToText(true));
  strcat(WebMessage,"&ExhaustFan="); strcat(WebMessage,fanSpeedToText(false)); 
  Serial.println(WebMessage);   
  Mqtt.publish(MqttPublish, WebMessage);
}

void mqttPublished(void* response) {
  addToLog("MQTT published");
}
