//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Light sensor with analog and digital outputs

//Pins
const int DigitalPin = 4;       // D0 digital input - LM393 light sensor
const int AnalogPin = A0; // A0 analog input - LM393 light sensor

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for testing: Light sensor with analog and digital outputs"));
  pinMode(DigitalPin, INPUT);
  pinMode(AnalogPin, INPUT);
}

void loop()
{
  bool isLightOn = !digitalRead(DigitalPin); // read the input pin: 1- light detected , light detected. Had to invert(!) signal from the sensor to make more sense.
  if (isLightOn)
    Serial.println(F("Light detected"));
  else
    Serial.println(F("Light not detected"));
  Serial.print(F("Light intensity - "));
  Serial.println(1023 - analogRead(AnalogPin));
  delay(2000);
}
