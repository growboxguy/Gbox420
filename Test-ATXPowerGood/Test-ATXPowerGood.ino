//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: ATX PowerGood pin output over an inverting optocoupler 

//Pins
  const byte ATXPowerGoodInPin = 35; //5V signal from ATX powersupply, LOW if DC power output is OK

//Global variables
  bool DCPowerOK = true; //DC voltage present

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //wifi console output
  pinMode(ATXPowerGoodInPin, INPUT_PULLUP);
}

void loop() {  // put your main code here, to run repeatedly:
Serial.println(F("Reading ATX PowerGood pin")); 
DCPowerOK = !digitalRead(ATXPowerGoodInPin); //inverting the reading to compensate the inverting optocoupler
if(DCPowerOK) Serial.println("ATX PowerGood signal OK");
else Serial.println("ATX PowerGood signal NOT OK");

delay(5000); //delay 5 seconds
}
