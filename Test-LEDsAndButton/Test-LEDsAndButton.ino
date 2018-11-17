//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testig: PC case LEDs and Power button

//Pins
const int PowerLEDOutPin = 46;  // Digital output - PC case Power LED
const int HddLEDOutPin = 47; // Digital output - PC case HDD LED
const int PowerButtonInPin = 48;  //Power button 

//Global variables
bool isLightOn = true;  //Startup status for lights: True-ON / False-OFF

void setup(){
  pinMode(PowerLEDOutPin, OUTPUT);
  pinMode(HddLEDOutPin, OUTPUT);
  pinMode(PowerButtonInPin,  INPUT_PULLUP); //Enables internal pullup resistor
  digitalWrite(PowerLEDOutPin, LOW); //OFF, without this LED would be randomly on or off when the board is powered on
  digitalWrite(HddLEDOutPin, LOW);
}
 
void loop(){
  if(!digitalRead(PowerButtonInPin))isLightOn = !isLightOn;  //If the power button is held invert the light status
  if(isLightOn) digitalWrite(PowerLEDOutPin, LOW); else digitalWrite(PowerLEDOutPin, HIGH);  
  flash(100); flash(100); flash(100); // S  
  delay(200);
  flash(250); flash(250); flash(250); // O
  delay(200);
  flash(100); flash(100); flash(100); // S  
  delay(500);
}

void flash(int duration){
digitalWrite(HddLEDOutPin, HIGH);
delay(duration);
digitalWrite(HddLEDOutPin, LOW);
delay(duration);
}


