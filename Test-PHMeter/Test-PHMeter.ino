//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: PH meter.. code incomplete, calibration part missing

//Pins
int PHMeterInPin = A15; //Po analog port - PH meter

void setup() {
  Serial.begin(115200);
}

void loop() {
  int measure = analogRead(PHMeterInPin);
  Serial.print("PH: ");
  Serial.print(measure);

  double voltage = 5 / 1024.0 * measure; //classic digital to voltage conversion
  Serial.print("\tVoltage: ");
  Serial.print(voltage, 3);

  // PH_step = (voltage@PH7 - voltage@PH4) / (PH7 - PH4)
  // PH_probe = PH7 - ((voltage@PH7 - voltage@probe) / PH_step)
  float Po = 7 + ((2.5 - voltage) / 0.18);
  Serial.print("\tPH: ");
  Serial.print(Po, 3);

  Serial.println("");
  delay(2000);
}
