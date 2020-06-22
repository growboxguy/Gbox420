//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for calibrating PH sensor

//Pins
int PHMeterInPin = A0; //Po analog port - PH meter

//Global constants
float PHCalibrationSlope = -0.031332;     //Update this to your own calibration values
float PHCalibrationIntercept = 22.637144; //Update this to your own calibration values

float TestSolution1PH = 4.01;
float TestSolution2PH = 6.86;

//Global variables
float PH;
float TestSolution1Reading = 0;
float TestSolution2Reading = 0;
bool CalibrationComplete = false;
bool FirstTestSolution = true;
char ShortMessage[32] = "";

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for calibrating PH sensor"));
  Serial.println();
  Serial.print(F("Insert the PH sensor into the calibration solution with PH: "));
  Serial.println(TestSolution1PH);
  Serial.println(F("Starting to measure PH, send any Serial input when the readings stabilize..."));
}

void loop()
{
  if(!CalibrationComplete && Serial.available())
  {
    Serial.print(F("Serial input received: "));
    Serial.print(Serial.readString());
        
    Serial.print(F("  Saving reading at PH: "));
    if(FirstTestSolution)
    {
      Serial.println(TestSolution1PH);
      TestSolution1Reading = 594.5; //readPH();
      Serial.print(F("Insert the PH sensor into the calibration solution with PH: "));
      Serial.println(TestSolution2PH);
      FirstTestSolution = false;         
    }
    else{
      Serial.println(TestSolution2PH);
      TestSolution2Reading = 503.15; //readPH();
      Serial.println();
      Serial.println(F("Collecting readings done: "));
      Serial.print(TestSolution1PH);
      Serial.print(F(" - "));
      Serial.println(TestSolution1Reading);
      Serial.print(TestSolution2PH);
      Serial.print(F(" - "));
      Serial.println(TestSolution2Reading);
      Serial.println();

      float PHCalibrationSlope= (TestSolution2PH - TestSolution1PH) / (TestSolution2Reading - TestSolution1Reading);
      float PHCalibrationIntercept= PHCalibrationSlope * -1 * TestSolution1Reading + TestSolution1PH;    
      Serial.println(F("Calibration complete:"));
      Serial.print(F("  Slope: "));
      Serial.println(toText_floatDecimals(PHCalibrationSlope));
      Serial.print(F("  Intercept: "));
      Serial.println(toText_floatDecimals(PHCalibrationIntercept));
      Serial.println();
      CalibrationComplete = true;
    }    
  }
  readPH(); 
  if(CalibrationComplete){
    delay(3000); 
  }
}

float readPH()
{
  float Reading = 0;
  for (byte i = 0; i < 20; i++)
  {
    Reading += analogRead(PHMeterInPin);
    delay(100);
  }
  Reading = Reading / 20; //Calculates average
  Serial.print("  Analog reading: ");
  Serial.print(Reading);
  PH = PHCalibrationSlope * Reading + PHCalibrationIntercept; //equation of the line
  Serial.print(" , PH: ");
  Serial.println(PH);
  return Reading;
}

char *toText_floatDecimals(float Number)
{  
  if (isnan(Number))
    Number = -1.0;
  dtostrf(Number, 8, 6, ShortMessage); ///minimum 8 char total length, with 6 decimals
  return ShortMessage;
}
