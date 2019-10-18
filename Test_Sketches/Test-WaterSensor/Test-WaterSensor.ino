//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Contactless water level sensors

//Pins
  const byte WaterCriticalInPin = A4; //Water sensor1
  const byte WaterLowInPin = A5;     //Water sensor2
  const byte WaterMediumInPin = A6; //Water sensor3
  const byte WaterFullInPin = A7; // Water sensor4

//Global variables
  bool isWaterAboveCritical = false; //Water sensor, true if level reached
  bool isWaterAboveLow = false; //Water sensor, true if level reached
  bool isWaterAboveMedium = false; //Water sensor, true if level reached
  bool isWaterAboveFull = false; //Water sensor, true if level reached

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(WaterCriticalInPin, INPUT_PULLUP); //pull input pin high by default for NPN sensor signal
  pinMode(WaterLowInPin, INPUT_PULLUP); //when water is detected the signal wire connects to ground  pulling the input low
  pinMode(WaterMediumInPin, INPUT_PULLUP);
  pinMode(WaterFullInPin, INPUT_PULLUP);
}

void loop() {  // put your main code here, to run repeatedly:
    isWaterAboveCritical = !digitalRead(WaterCriticalInPin); //high: no water detected, low: water detected
    isWaterAboveLow = !digitalRead(WaterLowInPin); //! inverted signal to make more sense
    isWaterAboveMedium = !digitalRead(WaterMediumInPin);
    isWaterAboveFull = !digitalRead(WaterFullInPin);
    
    Serial.print(F("Reservoir level: E["));
    if(isWaterAboveCritical) Serial.print(F("░")); else Serial.print(F("▁"));
    if(isWaterAboveLow) Serial.print(F("░")); else Serial.print(F("▁"));
    if(isWaterAboveMedium) Serial.print(F("░")); else Serial.print(F("▁"));
    if(isWaterAboveFull) Serial.print(F("░")); else Serial.print(F("▁"));
    Serial.println(F("]F"));
    delay(1000);
}
