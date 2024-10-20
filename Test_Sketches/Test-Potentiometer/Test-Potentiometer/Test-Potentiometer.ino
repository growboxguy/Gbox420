//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing 10kΩ potentiometer
//Potentiometer legs:
// -increase clockwise: Left: GND, Center: any analog pin, Right: +5V
// -increase counterclockwise: Left: +5V, Center: any analog pin, Right: GND

const byte AnalogPin = A0;

void setup() {
Serial.begin(115200);
Serial.println(F("Sketch for testing: 10kΩ potentiometer"));
}

void loop() {
int sensorValue = analogRead(AnalogPin); //take a reading
Serial.print(sensorValue);
Serial.print(F(" ("));
Serial.print(map(sensorValue, 0, 1023, 0, 100));
Serial.println(F("%)"));
delay(250); //every 0,25 seconds 
}
