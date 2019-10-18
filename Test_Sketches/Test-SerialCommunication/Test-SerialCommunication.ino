//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Arduino and ESP-link firmware communication
//This sketch assumes you already have the ESP8266 chip flashed with the ESP-Link firmware, 
//and connected it to the internet, as described in https://sites.google.com/site/growboxguy/arduino/esp8266
//If you get stuck at ESP-Link starting....... message means the Arduino cannot get in sync with the ESP8266 chip over Serial3 port.
//Make sure to enabled the SLIP protocol under REST/MQTT tab on the ESP-link website!

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //wifi console output
}

void loop() {  // put your main code here, to run repeatedly:
  Serial.println("Marko");
  Serial3.println("Polo");
  delay(5000); 
}
