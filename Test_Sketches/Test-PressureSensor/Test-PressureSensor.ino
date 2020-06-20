//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for getting Pressure sensor offset

//Pins
const byte PressureSensorInPin = A7; //analog port - Pressure sensor yellow cable

//Global variables
float Offset = 0.57; //Voltage reading with 0 Pressure
float Ratio = 2.7;   //Pressure sensor voltage to pressure ratio
float Pressure;
float PressurePSI;

void setup()
{
  Serial.begin(115200); 
  Serial.println(F("Sketch for getting Pressure sensor offset")); 
  pinMode(PressureSensorInPin, INPUT);
  getOffset();
  Serial.print(F("Ratio: "));
  Serial.println(Ratio);
  Serial.println();
}

void loop()
{
  float Reading = 0;
  for (byte i = 0; i < 50; i++)
  {
    Reading += analogRead(PressureSensorInPin);
    delay(100);
  }
  Reading = Reading / 50;
  float Voltage = ((float)Reading) * 5 / 1024;   //Sensor output voltage
  Pressure = (Ratio * (Voltage - Offset)); // unit: bar / 100kPa
  PressurePSI = Pressure * 14.5038;        //unit: PSI

  Serial.print(F("Sensor Reading: "));
  Serial.println(Reading);
  Serial.print(F("Voltage: "));
  Serial.print(Voltage);
  Serial.print(F("V, Offset compensated:"));
  Serial.print(Voltage - Offset);
  Serial.println(F("V"));
  Serial.print(F("Pressure: "));
  Serial.print(Pressure, 1);
  Serial.print(F(" bar / "));
  Serial.print(PressurePSI, 1);
  Serial.println(F(" psi"));
  Serial.println();
}

void getOffset()
{
  Serial.println(F("Calibrating 0 pressure offset for 5 sec, make sure the system is not under pressure..."));
  float sum = 0;
  for (byte i = 0; i < 50; i++)
  {
    sum += analogRead(PressureSensorInPin);
    delay(100);
  }
  Offset = (sum / 50) * 5 / 1024;
  Serial.print(F("Offset: "));
  Serial.println(Offset); //expected ~0.5V  
}
