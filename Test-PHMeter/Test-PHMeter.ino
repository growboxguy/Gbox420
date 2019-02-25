//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: PH meter

//Pins
int PHMeterInPin = A3; //Po analog port - PH meter

//Global constants
float PHCalibrationSlope = -0.031332;      //Update this to your own calibration values
float PHCalibrationIntercept = 22.637144;  //Update this to your own calibration values

//Global variables
float PH;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting to take PH readings...");
}

void loop() {
readPH();
}

void readPH(){
  float  Reading=0;
for(byte i=0;i<200;i++) { 
   Reading+=analogRead(PHMeterInPin);
   delay(100);
}
Reading = Reading /200; //Calculates average
Serial.print("Analog reading: "); Serial.println(Reading);
PH = PHCalibrationSlope*Reading + PHCalibrationIntercept;  //equation of the line
Serial.print("PH: "); Serial.println(PH);
Serial.println("");
}
