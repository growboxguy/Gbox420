//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for reading TDS using TDS meter V1.0 for Arduino

const byte Pin = A1;              //Analog port of the TDS sensor (A)
const byte WaterTemperature = 22; //Fake water temp for compensating sensor reading

void setup()
{
  Serial.begin(115200);
  pinMode(Pin, INPUT);
}

void loop()
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
  float TDS = (133.42 * pow(compensatedVoltage, 3) - 255.86 * pow(compensatedVoltage, 2) + 857.39 * compensatedVoltage) * 0.5;
  Serial.print(F("TDS Value:"));
  Serial.print(TDS, 0);
  Serial.println(F("ppm"));
}
