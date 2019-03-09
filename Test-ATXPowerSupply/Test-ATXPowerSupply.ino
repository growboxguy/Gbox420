//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Turning ATX Power ON/OFF and reading ATX PowerGood status 

//Pins
  const byte ATXPowerONOutPin = 34; //Turns ATX power supply on by connecting ATX PowerON pin to GND through an optocupler
  const byte ATXPowerGoodInPin = 35; //5V signal from ATX powersupply, inverted by optocoupler: LOW if DC power output is OK

//Global variables
  bool DCPowerOK = true; //DC voltage present
  bool ATXPowerSupplyOn = true; //ATX power supply ON(true) or OFF(false)

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //wifi console output
  pinMode(ATXPowerGoodInPin, INPUT_PULLUP);
  pinMode(ATXPowerONOutPin, OUTPUT);
  digitalWrite(ATXPowerONOutPin, LOW);  //OFF: Avoids random ON/OFF state at startup
}

void loop() {  // put your main code here, to run repeatedly:
Serial.print(F("Reading ATX PowerGood pin: ")); 
DCPowerOK = !digitalRead(ATXPowerGoodInPin); //inverting the reading to compensate the inverting optocoupler
if(DCPowerOK) Serial.println("ATX PowerGood signal OK");
else Serial.println("ATX PowerGood signal NOT OK");

if(ATXPowerSupplyOn) {
  digitalWrite(ATXPowerONOutPin, HIGH);
  Serial.println(F("Turning ATX power supply ON"));
}
else {
  digitalWrite(ATXPowerONOutPin, LOW);
  Serial.println(F("Turning ATX power supply OFF"));  
}

delay(15000); //delay 15 seconds
Serial.println();
ATXPowerSupplyOn = !ATXPowerSupplyOn;  //Flip the ATX ON status
}
