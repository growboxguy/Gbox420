//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Light sensor with analog and digital outouts

//Pins
const int LightSensorInPin = 50;     // D0 digital input - LM393 light sensor
const int LightSensorAnalogInPin = A0;     // A0 analog input - LM393 light sensor

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LightSensorInPin, INPUT);
  pinMode(LightSensorAnalogInPin, INPUT);
}

void loop() {  // put your main code here, to run repeatedly:
  bool isLightOn = !digitalRead(LightSensorInPin);     // read the input pin: 1- light detected , light detected. Had to invert(!) signal from the sensor to make more sense.
  if(isLightOn)Serial.println("Light detected");
  else Serial.println("Light not detected");
  Serial.print("Light intensity - ");Serial.println(1023 - analogRead(LightSensorAnalogInPin));  
  delay(1000);
}
