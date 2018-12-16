//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: PH meter

//Pins
int PHMeterInPin = A15; //Po analog port - PH meter

void setup() {
  Serial.begin(115200);
  Serial.print("Starting to take PH readings...");
}

void loop() {
float  Reading=0;
for(byte i=0;i<200;i++) { 
   Reading+=analogRead(PHMeterInPin);
   delay(100);
}
Reading = Reading /200; //Calculates average
Serial.print("Analog reading: "); Serial.println(Reading);
float PH = -0.031308*Reading + 22.612698;  //equation of the line
Serial.print("PH: "); Serial.println(PH);
Serial.println("");
}
