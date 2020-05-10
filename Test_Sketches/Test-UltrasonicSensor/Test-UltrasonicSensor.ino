//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: HC-SR04 Ultrasonic sensor (Distance measuring using sound - For reservoir water level tracking)

//Pins
const byte TriggerOutPin = 4; // HC-SR04 Trigger pin
const byte EchoInPin = 5; // HC-SR04 Echo pin

//Global Variables
long duration;
float distanceCm;  //Stores the measured distance in cm
float distanceInch;  //Stores the measured distance in inch

void setup()
{
  Serial.begin(115200);
  pinMode(TriggerOutPin, OUTPUT);
  pinMode(EchoInPin, INPUT);
}

void loop()
{
  digitalWrite(TriggerOutPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TriggerOutPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerOutPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(EchoInPin, HIGH);
  
  // Calculating the distance
  distanceCm= duration*0.034/2;
  distanceInch = duration*0.0133/2;
  
  // Prints the distance on the Serial Monitor
  Serial.print(F("Distance: "));
  Serial.print(distanceCm);
  Serial.print(F("cm / "));
  Serial.print(distanceInch);
  Serial.println(F("inch"));
  
  delay(2000); //Delay 2 sec.
}
