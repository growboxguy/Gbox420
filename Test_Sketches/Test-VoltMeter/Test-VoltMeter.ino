//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for voltage measuring using 1MOhm / 100KOhm voltage divider resistors

//Pins
const byte VoltageMeterInPin = A2; //PC speaker+ (red)

//Global constants
const float ActualArduino5V = 4.91;  //measure arduino 5V with a multimeter
const float DividingFactor = 10.9756;  //dividing factor = Calibration voltage / Voltage over 100KOhm resistor

//Global variables
float Voltage;

void setup()
{
 Serial.begin(115200);
}

void loop()
{
 readVoltage(); 
}

void readVoltage(){
  float Reading=0;
  for (byte i=0;i<100;i++) {
        Reading += analogRead(VoltageMeterInPin);
        delay(10);
  }   
  Voltage = (Reading / 100 * 5) / 1024.0 * DividingFactor;
  Serial.print(Voltage);
  Serial.println (" V");
}
