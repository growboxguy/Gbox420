//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: PH meter

//Pins
int PHMeterInPin = A3; //Po analog port - PH meter

//Global variables
float PH;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting to take PH readings...");
}

void loop() {
float  Reading=0;
for(byte i=0;i<200;i++) { 
   Reading+=analogRead(PHMeterInPin);
   delay(100);
}
Reading = Reading /200; //Calculates average
Serial.print("Analog reading: "); Serial.println(Reading);
PH = -0.031223*Reading + 23.376812;  //equation of the line
Serial.print("PH: "); Serial.println(PH);
Serial.println("");
}
