//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for calibrating PH sensor

//Pins
int PHMeterInPin = A0; //Po analog port - PH meter

float Slope = -0.031332;     //Update this to your own calibration values
float Intercept = 22.637144; //Update this to your own calibration values
const float TestSolution1PH = 4.01;
const float TestSolution2PH = 6.86;
bool OffsetCalibrationComplete = false;
bool PHCalibrationComplete = false;

//Global variables
float PH;
float TestSolution1Reading = 0;
float TestSolution2Reading = 0;
bool FirstTestSolution = true;
char ShortMessage[32] = "";

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for calibrating PH sensor"));
  Serial.println();
  if (!OffsetCalibrationComplete)
  {
    Serial.println(F("Offset calibration starting..."));
    Serial.println(F(" Disconnect the PH probe and short circuit the inside of the BNC connector with the outside"));
    Serial.println(F(" Adjust the offset potentiometer until the voltage reading shows 2.5V"));
    Serial.println(F(" Send any Serial input once the offset is set"));
  }
  else
  {
    Serial.println(F("PH calibration starting..."));
    Serial.print(F(" Insert the PH probe into the calibration solution with PH: "));
    Serial.println(TestSolution1PH);
    Serial.println(F(" Starting to measure PH, send any Serial input when the readings stabilize..."));
  }
}

void loop()
{
  if (!OffsetCalibrationComplete && Serial.available())
  {
    Serial.print(F("Serial input received: "));
    Serial.print(Serial.readString());
    Serial.println(F("Offset calibration complete"));
    Serial.println();
    Serial.println(F("PH calibration"));
    Serial.print(F("Re-connect the PH probe and insert it into the calibration solution with PH: "));
    Serial.println(TestSolution1PH);
    Serial.println(F("Starting to measure PH, send any Serial input when the readings stabilize..."));
    OffsetCalibrationComplete = true;
  }

  if (OffsetCalibrationComplete && !PHCalibrationComplete && Serial.available())
  {
    Serial.print(F("Serial input received: "));
    Serial.print(Serial.readString());

    Serial.print(F("  Saving reading at PH: "));
    if (FirstTestSolution)
    {
      Serial.println(TestSolution1PH);
      TestSolution1Reading = readPH();
      Serial.print(F("Insert the PH probe into the calibration solution with PH: "));
      Serial.println(TestSolution2PH);
      Serial.println(F("Starting to measure PH, send any Serial input when the readings stabilize..."));
      FirstTestSolution = false;
    }
    else
    {
      Serial.println(TestSolution2PH);
      TestSolution2Reading = readPH();
      Serial.println();
      Serial.println(F("Collecting readings done: "));
      Serial.print(TestSolution1PH);
      Serial.print(F(" - "));
      Serial.println(TestSolution1Reading);
      Serial.print(TestSolution2PH);
      Serial.print(F(" - "));
      Serial.println(TestSolution2Reading);
      Serial.println();

      float Slope = (TestSolution2PH - TestSolution1PH) / (TestSolution2Reading - TestSolution1Reading);
      float Intercept = Slope * -1 * TestSolution1Reading + TestSolution1PH;
      Serial.println(F("Calibration complete:"));
      Serial.print(F("  Slope: "));
      Serial.println(toText_floatDecimals(Slope));
      Serial.print(F("  Intercept: "));
      Serial.println(toText_floatDecimals(Intercept));
      Serial.println();
      PHCalibrationComplete = true;
    }
  }
  readPH();
  if (PHCalibrationComplete)
  {
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
  float Voltage = Reading * 5 / 1024;
  Serial.print(F("  Analog reading: "));
  Serial.print(Reading);
  Serial.print(F(", voltage: "));
  Serial.print(Voltage);
  PH = Slope * Reading + Intercept; //equation of the line
  Serial.print(F(" , PH: "));
  Serial.println(PH);
  return Reading;
}

char *toText_floatDecimals(float Number)
{
  if (isnan(Number))
    Number = -1.0;
  dtostrf(Number, 8, 6, ShortMessage); ///< minimum 8 char total length, with 6 decimals
  return ShortMessage;
}
