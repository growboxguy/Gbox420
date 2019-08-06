#include "Common.h" 
//#include "420Settings.h"  //for storing/reading defaults
#include "ELClient.h" //ESP-link
#include "ELClientWebServer.h" //ESP-link - WebServer API
#include "DHTSensor.h"
#include "Lights.h"


//TODO:
//addToLog - make Log class
//Convert Website to a Singleton class (Restricts the instantiation to one single instance) 
//Make alerting a class, every object will have a "bool StatusOK" variable inherited from Common, Alerting will go throught all subscribed classes and check status
//Subscribe to thread timer events using the attach method.
//sendEmailAlert implementation
//Sounds implementation
//Light sensor to separate object from Light

DHTSensor *InternalDHTSensor;
DHTSensor *ExternalDHTSensor;
Lights *GrowLights;
  
ELClient ESPLink(&Serial3);  //ESP-link. Both SLIP and debug messages are sent to ESP over Serial3
ELClientWebServer WebServer(&ESPLink); //ESP-link WebServer API

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //esp wifi console output
  loadSettings();
  
  InternalDHTSensor = new DHTSensor(&Common::MySettings.InternalDHTSensorPin,DHT22);  //passing: Pin and Sensor type(DHT22/DHT11)
  ExternalDHTSensor = new DHTSensor(&Common::MySettings.InternalDHTSensorPin,DHT22);  //passing: Pin and Sensor type(DHT22/DHT11)
  GrowLights = new Lights(&Common::MySettings.Light1RelayPin,&Common::MySettings.Light1Status,&Common::MySettings.Light1Brightness,&Common::MySettings.Light1TimerEnabled,&Common::MySettings.LightOnHour,Common::MySettings.LightOnMinute,&Common::MySettings.LightOffHour,&Common::MySettings.LightOffMinute);   //Passing MySettings members as references: Changes get written back to MySettings and saved to EEPROM. (byte *)(((byte *)&Common::MySettings) + offsetof(Settings, LightOnHour))

  //Initialize web connections
  ESPLink.resetCb = resetWebServer;  //Callback subscription: When wifi reconnects, restart the WebServer
  resetWebServer();  //reset the WebServer
  ESPLink.Process();  //Process any command from ESP-Link
}

void loop() { // put your main code here, to run repeatedly:
  InternalDHTSensor -> refresh();
  ESPLink.Process();
}
