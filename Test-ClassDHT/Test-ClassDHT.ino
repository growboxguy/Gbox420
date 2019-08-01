#include "DHTSensor.h"

DHTSensor *InternalDHTSensor;

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);
  char Message[512]; //initialize the temporary character buffer
  
  InternalDHTSensor = new DHTSensor(43,DHT22);  //passing: Pin and Sensor type(DHT22/DHT11)

}

void loop() { // put your main code here, to run repeatedly:
  InternalDHTSensor -> refresh();
  Serial.println(F("Printing floats:"));
  Serial.println(InternalDHTSensor -> getTemp());  
  Serial.println(InternalDHTSensor -> getHumidity());
  Serial.println(F("Printing toText:"));
  Serial.println(InternalDHTSensor -> getTempText());
  Serial.println(InternalDHTSensor -> getHumidityText());
  delay(5000);
}
