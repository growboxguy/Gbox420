//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Optocoupler based HLG LED driver dimming
//HLG LED driver dimmin by applying additive 10V PWM signal (frequency range 100Hz ~ 3KHz)
//Using: 490Hz PWM signal, Duty cycle is inverted by the optocoupler!

//Pins
const byte DimmingPin = 11;    //Optocoupler PC817 base over 680ohm resistor
const byte LightRelayPin = 29; //Pin that controls the relay turning the lights ON/OFF

//Global constants
const byte DimmingLimit = 6; //Does not allow the dimming to go below 6%

//Global variables
byte Brightness = 0;       //Light intensity: Adjusts PWM duty cycle
bool isGettingHigh = true; //Direction to increase or decrease brightness

void setup()
{ // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(DimmingPin, OUTPUT);
  pinMode(LightRelayPin, OUTPUT);
  Serial.println(F("Turing on lights and starting dimming test..."));
  digitalWrite(LightRelayPin, LOW); ///True turns relay ON (LOW signal activates Relay)
}

void loop()
{ // put your main code here, to run repeatedly:
  while (0 <= Brightness && Brightness <= 100)
  {
    analogWrite(DimmingPin, map(Brightness, 0, 100, int(255 * (100 - DimmingLimit) / 100.0f), 0)); ///mapping brightness to duty cycle without allowing it to dimm below DimminGlimit. Example 1: Brightness 100 -> PWM duty cycle will be 0% on Arduino side, 100% on LED driver side. Example2: Mapping Brightness 0 with Dimming limit 6% ->  int(255*((100-6)/100)) ~= 239 AnalogWrite (94% duty cycle on Arduino Side, 6% in Driver dimming side) https:///www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
    if (Brightness % 10 == 0)                                                                      //modulo division, https://www.arduino.cc/reference/en/language/structure/arithmetic-operators/modulo/
    {
      Serial.print(Brightness);
      Serial.println("%");
    }
    if (isGettingHigh)
      Brightness++;
    else
      Brightness--;
    delay(200); //Wait 0.2seconds between 1% change
  }

  //When the while loop above breaks the Brightness is out of the 0 to 100 bounds
  if (isGettingHigh)
  {
    Serial.println("MAX brightness reached");
    Brightness = 100; //Set Brightness back within bounds
  }
  else
  {
    Serial.println("MIN brightness reached");
    Brightness = 0; //Set Brightness back within bounds
  }
  isGettingHigh = !isGettingHigh; // flip the direction
  delay(5000);                    //Wait 5 seconds
}
