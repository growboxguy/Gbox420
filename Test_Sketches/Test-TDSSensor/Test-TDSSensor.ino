//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for reading TDS using TDS meter V1.0 for Arduino

//Pins
const byte Pin = A1; //Analog port of the TDS sensor (A)

const byte WaterTemperature = 22;   //Fake water temp for compensating sensor reading
const float TestSolution1TDS = 567; //< Known solution1 TDS
const float TestSolution2TDS = 2440; //< Known solution2 TDS
bool TDSCalibrationComplete = false;

//Starting calibration values
float Slope = 6.638663;     //Update this to your own calibration values
float Intercept = -372.23798; //Update this to your own calibration values

//Global variables
float TDS;
float TestSolution1Reading = 0;
float TestSolution2Reading = 0;
bool FirstTestSolution = true;
char ShortMessage[32] = "";

void setup()
{
  Serial.begin(115200);
  pinMode(Pin, INPUT);
  Serial.println(F("TDS calibration starting..."));
  Serial.print(F(" Insert the TDS probe into the calibration solution with known TDS of: "));
  Serial.println(TestSolution1TDS);
  Serial.println(F(" Starting to measure TDS, send any Serial input when the readings stabilize..."));
}

void loop()
{
  if (!TDSCalibrationComplete && Serial.available())
  {
    Serial.print(F("Serial input received: "));
    Serial.print(Serial.readString());

    Serial.print(F("  Saving reading at TDS: "));
    if (FirstTestSolution)
    {
      Serial.println(TestSolution1TDS);
      TestSolution1Reading = readTDS();
      Serial.print(F("Insert the TDS probe into the second calibration solution with known TDS of: "));
      Serial.println(TestSolution2TDS);
      Serial.println(F("Starting to measure TDS, send any Serial input when the readings stabilize..."));
      FirstTestSolution = false;
    }
    else
    {
      Serial.println(TestSolution2TDS);
      TestSolution2Reading = readTDS();
      Serial.println();
      Serial.println(F("Collecting readings done: "));
      Serial.print(TestSolution1TDS);
      Serial.print(F(" - "));
      Serial.println(TestSolution1Reading);
      Serial.print(TestSolution2TDS);
      Serial.print(F(" - "));
      Serial.println(TestSolution2Reading);
      Serial.println();

      float Slope = (TestSolution2TDS - TestSolution1TDS) / (TestSolution2Reading - TestSolution1Reading);
      float Intercept = Slope * -1 * TestSolution1Reading + TestSolution1TDS;
      Serial.println(F("Calibration complete:"));
      Serial.print(F("  Slope: "));
      Serial.println(toText_floatDecimals(Slope));
      Serial.print(F("  Intercept: "));
      Serial.println(toText_floatDecimals(Intercept));
      Serial.println();
      TDSCalibrationComplete = true;
    }
  }
  readTDS();
  if (TDSCalibrationComplete)
  {
    delay(3000);
  }
}

float readTDS()
{
  float Reading = 0;
  for (byte i = 0; i < 25; i++) //Take 25 readings
  {
    Reading += analogRead(Pin);
    delay(100);
  }
  Reading = Reading / 25;                      //Average of 25 readings
  float Voltage = ((float)Reading) * 5 / 1024; //Convert average to voltage
  float compensatedVoltage = Voltage / (1.0 + 0.02 * (WaterTemperature - 25.0));
  float CalculatedTDS = (133.42 * pow(compensatedVoltage, 3) - 255.86 * pow(compensatedVoltage, 2) + 857.39 * compensatedVoltage) * 0.5;
  float CalibratedTDS = Slope * CalculatedTDS + Intercept;
  Serial.print(F("  Analog reading: "));
  Serial.print(Reading);
  Serial.print(F(", voltage: "));
  Serial.print(Voltage);
  Serial.print(F(", Calculated TDS:"));
  Serial.print(CalculatedTDS, 0);
  Serial.print(F("ppm"));
  Serial.print(F(", Calculated TDS:"));
  Serial.print(CalibratedTDS, 0);
  Serial.println(F("ppm"));
  return Reading;
}

char *toText_floatDecimals(float Number)
{
  if (isnan(Number))
    Number = -1.0;
  dtostrf(Number, 8, 6, ShortMessage); ///< minimum 8 char total length, with 6 decimals
  return ShortMessage;
}
