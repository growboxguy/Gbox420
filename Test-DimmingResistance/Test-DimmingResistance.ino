//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Digital potentiometer

//Pins
const int PotCSOutPin = 34; //Digital output - X9C104 digital potentiometer CS 
const int PotUDOutPin = 32;  // Digital output - X9C104 digital potentiometer UD
const int PotINCOutPin = 33;  // Digital output - X9C104 digital potentiometer INC

//Global variables
bool isGettingHigh = true;  //true-Increse, false-Decrease resistance

void setup() {  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(PotCSOutPin, OUTPUT);
pinMode(PotUDOutPin, OUTPUT);
pinMode(PotINCOutPin, OUTPUT);
digitalWrite(PotCSOutPin,LOW); //CS input is low when the chip is active
Serial.println("Reseting to Step 0...");
for (int steps = 0; steps < 100; steps++) {stepOne();}  //Sets the digital potentiometer to Step 0 irregardless what the stored startup value is
isGettingHigh = true;
}

void loop() { // put your main code here, to run repeatedly:
int StepCounter = 0;
while(StepCounter <= 100){    //the X9C104 digital potentiometer has 99 steps
  stepOne();      //step in one direction (Initially upwards) 
  if(StepCounter % 10 == 0)  //modulo division, https://www.arduino.cc/reference/en/language/structure/arithmetic-operators/modulo/
   { 
    if(isGettingHigh){ Serial.print(StepCounter); Serial.println("%");}
    else { Serial.print(100-StepCounter); Serial.println("%");}
   }
  StepCounter++;
}
if(isGettingHigh) Serial.println("MAX reached");
else Serial.println("MIN reached");
isGettingHigh= !isGettingHigh;  // flip the direction
delay(5000);
}

void stepOne(){
digitalWrite(PotUDOutPin,isGettingHigh);  //true=HIGH=increase brightness
digitalWrite(PotINCOutPin,HIGH);   //signal the change to the chip
delay(50);  //delay oversized for testing, delayMicroseconds(10); would be enough 
digitalWrite(PotINCOutPin,LOW);
delay(50);
}

void store(){   //store current potentiometer value in the X9C104 memory for the next power on. Write durability is only 100.000 writes, use with caution
digitalWrite(PotINCOutPin,HIGH);
digitalWrite(PotCSOutPin,HIGH);
delay(50);
digitalWrite(PotCSOutPin,LOW);
digitalWrite(PotINCOutPin,LOW);
}
