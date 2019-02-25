//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Optocoupler based HLG LED driver dimming
//HLG LED driver dimmin by applying additive 10V PWM signal (frequency range 100Hz ~ 3KHz)
//Using: 490Hz PWM signal, Duty cycle is inverted by the optocoupler!
//PWM dimming duty cycle 0 (No dimming) - 235 (Dimm to 8%, Lowest allowed dimming by Meanweell is 6%)

//Pins
const byte OptocouplerBase = 9; //Optocoupler PC817 base over 1K resistor

//Global variables
byte LightBrightness = 0;   //Light intensity: Adjusts PWM duty cycle
bool isGettingHigh = true;

void setup() {  // put your setup code here, to run once:
 Serial.begin(115200);
 pinMode(OptocouplerBase, OUTPUT);
 Serial.println("Starting dimming test...");
}

void loop() { // put your main code here, to run repeatedly:
while(0 <= LightBrightness && LightBrightness <= 100){    
  analogWrite(OptocouplerBase, map(LightBrightness,0,100,235,0) ); //mapping 0% output to 92% (235/255) duty cycle, and 100% output to 0% (0/255) duty cycle
  if(LightBrightness % 10 == 0)  //modulo division, https://www.arduino.cc/reference/en/language/structure/arithmetic-operators/modulo/
   { 
    Serial.print(LightBrightness); Serial.println("%");
   }
  if(isGettingHigh) LightBrightness++;
  else LightBrightness--;
  delay(200);
}
if(isGettingHigh){ Serial.println("MAX brightness reached"); LightBrightness--;}
else {Serial.println("MIN brightness reached"); LightBrightness++;}
isGettingHigh= !isGettingHigh;  // flip the direction
delay(10000);
}
