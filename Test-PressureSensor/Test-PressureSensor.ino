//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Pressure sensor 0 - 1.2mPa , 0.5V - 4.5V sensor output

//Pins
const byte PressureSensorInPin = A1; //analog port - Pressure sensor

//Global variables
int PressureSensorOffSet = 1023 ;  //Lowest analog read value with 0 pressure
int Reading;
float Voltage;
float Pressure;
float PressurePSI;

void setup() {
  Serial.begin(115200);
  pinMode(PressureSensorInPin, INPUT);
  calibratePressureSensor();
}

void loop()
{
  Reading = analogRead(PressureSensorInPin) ;     //Sensor output reading 
  Voltage = Reading * 5 / 1024 ;     //Sensor output voltage
  Pressure = (Reading - PressureSensorOffSet) * 12 / (921 - PressureSensorOffSet);  //1.2mPa = 12 bar
  PressurePSI = (Reading - PressureSensorOffSet) * 174 / (921 - PressureSensorOffSet);  //1.2mPa = 174 psi

  Serial.print("Reading: "); Serial.println(Reading);
  Serial.print("Voltage: "); Serial.print(Voltage); Serial.println("V");
  Serial.print("Pressure: "); Serial.print(Pressure, 1); Serial.println(" bar");
  Serial.print("Pressure: "); Serial.print(PressurePSI, 1); Serial.println(" KPa");
  Serial.println();
  delay(1000);
}

void calibratePressureSensor(){
  for(byte i = 0; i<100;i++){
  Reading = analogRead(PressureSensorInPin) ;  
  if(Reading<PressureSensorOffSet) PressureSensorOffSet = Reading;
  delay(10);
  }
  Serial.print("Pressure sensor calibrated, offset: "); Serial.println(PressureSensorOffSet);
}

