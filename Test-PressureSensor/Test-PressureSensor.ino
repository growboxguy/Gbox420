//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: AeroPressure sensor 0 - 1.2mPa , 0.5V - 4.5V sensor output

//Pins
const byte AeroPressureSensorInPin = A1; //analog port - AeroPressure sensor

//Global variables
float AeroOffset = 0.5 ;  //Lowest voltage value with 0 AeroPressure
float AeroPressure;
float AeroPressurePSI;

void setup() {
  Serial.begin(115200);
  pinMode(AeroPressureSensorInPin, INPUT);
  calibrateAeroPressureSensor();
}

void loop()
{
  float  Reading=0;
  for(byte i=0;i<50;i++) { 
   Reading+=analogRead(AeroPressureSensorInPin);
   delay(20);
  }
  Reading = Reading /50;
  float Voltage = ((float)Reading) * 5 / 1024 ;     //Sensor output voltage
  AeroPressure = (2.7*(Voltage-AeroOffset)); // unit: bar / 100kPa
  AeroPressurePSI = AeroPressure * 14.5038;  //psi

  Serial.print("Reading: "); Serial.println(Reading);
  Serial.print("Voltage: "); Serial.print(Voltage); Serial.println("V");
  Serial.print("Offset: "); Serial.print(AeroOffset); Serial.print("V, Offset compensated:");Serial.print(Voltage-AeroOffset); Serial.println("V");
  Serial.print("Pressure: "); Serial.print(AeroPressure, 1); Serial.print(" bar / "); Serial.print(AeroPressurePSI, 1); Serial.println(" psi");
  Serial.println();
  delay(2000);
}

void calibrateAeroPressureSensor(){
  float sum = 0;
  for(byte i = 0; i<50;i++){
  sum += analogRead(AeroPressureSensorInPin) ; 
  delay(10);
  }  
  AeroOffset = (sum/50)*5/1024;
  Serial.print("Pressure sensor calibrated, offset voltage: "); Serial.println(AeroOffset); //expected ~0.5V
}

