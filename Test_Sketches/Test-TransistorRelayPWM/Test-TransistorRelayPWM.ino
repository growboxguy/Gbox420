//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Transistor DC relay

//Pins
const byte Pin = 3;                //Transistor relay Port S
const byte DutyCycleLowLimit = 20; //Limit the lowest speed to 20%, below this point the motor might not turn
byte DutyCycle = 0;                //Speed between 0 and 100%
bool isGettingHigh = true;         //Direction to increase or decrease speed during the test

void setup()
{ // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(F("Sketch for testing: Transistor DC relay - PWM motor speed control "));

  pinMode(Pin, OUTPUT);
  digitalWrite(Pin, LOW); //Start in off state
}

void loop()
{ // put your main code here, to run repeatedly:
  while (0 <= DutyCycle && DutyCycle <= 100)
  {
    analogWrite(Pin, map(DutyCycle, 0, 100, int(255 * (DutyCycleLowLimit / 100.0f)), 255)); ///< mapping motor speed to duty cycle https:///< www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
    if (DutyCycle % 10 == 0)                                                                //modulo division, https://www.arduino.cc/reference/en/language/structure/arithmetic-operators/modulo/
    {
      Serial.print(DutyCycle);
      Serial.println("%");
    }
    if (isGettingHigh)
      DutyCycle++;
    else
      DutyCycle--;
    delay(200); //Wait 0.2seconds between 1% change
  }

  if (isGettingHigh)
  {
    Serial.println("MAX speed reached");
    DutyCycle = 100; //Set Brightness back within bounds
  }
  else
  {
    Serial.println("MIN speed reached");
    DutyCycle = 0; //Set Brightness back within bounds
  }
  isGettingHigh = !isGettingHigh; // flip the direction
  delay(5000);                    //Wait 5 seconds
}
