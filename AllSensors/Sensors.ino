void readSensors(){  //Sensor readings  
    Humidity = dht.readHumidity();
    BoxTempC= dht.readTemperature();
    BoxTempF = BoxTempC *1.8 + 32;
    isBright = !digitalRead(LightSensorInPin);     // read the input pin: 1- light detected , 0 - no light detected. Had to invert signal from the sensor to make more sense.
    LightReading = 1023 - analogRead(LightSensorAnalogInPin);
    LightReadingPercent = map(LightReading,MinLightReading,MaxLightReading,0,100);
    Voltage = PowerSensor.voltage(PowerSensorIP);
    Current = PowerSensor.current(PowerSensorIP);
    Wattage = PowerSensor.power(PowerSensorIP);
    Energy = PowerSensor.energy(PowerSensorIP) / 1000;  //total power consumption in kWh
    Moisture = analogRead(AMoistureSensorPin);    //map(output_value,MAX,MIN,0,100);  calibrate it based on the obsereved highest and lowers reads
    ReadPH();
    WaterAboveCritical = !digitalRead(WaterCriticalInPin);
    WaterAboveLow = !digitalRead(WaterLowInPin);
    WaterAboveMedium = !digitalRead(WaterMediumInPin);
    WaterAboveFull = !digitalRead(WaterFullInPin);
}


//***PH METER***
//Global variables for ReadPH funciton
int SensorValue = 0; 
unsigned long int Average;
int Results[10];

void ReadPH(){
 for(int i=0;i<10;i++) { //take 10 readings
  Results[i]=analogRead(PHInPin);
 }
Average=0;
for(int i=0;i<10;i++) 
  Average+=Results[i];  //average of the 10 reads
float PHVoltage=(float)Average*5.0/1024/10;
PH = -5.70 * PHVoltage + 21.34;
}


//***8 digit display***
//Global variables for updateDisplay funciton
int CurrentStep = 0;
const byte degreeSign = B11100011; //Used at Temp step
void updateDisplay() { //Cycles through the different phases
    display.clear();   
    switch (CurrentStep) {
    case 0:
      display.print(F("Humi"));
      break;
    case 1:
      display.setColonOn(1);display.printDualCounter((int)Humidity , 100 * (Humidity - (int)Humidity));display.setColonOn(0);      
      break;
    case 2:
      display.print(F("Temp"));
      break;
    case 3:
      display.setColonOn(1);display.printDualCounter((int)BoxTempC , 100 * (BoxTempC - (int)BoxTempC));display.printRaw(degreeSign, 3);display.setColonOn(0);
      break;
    case 4:
      display.print(F("LGHT"));
      break;
    case 5:
      if(isLightOn) display.print(F("On")); else display.print(F("Off"));
      break;
    case 6:
      display.print(F("Watt"));
      break;
    case 7:
      display.print((int)Wattage);
      break;      
    case 8:
      display.print(F("Sum"));
      break;
    case 9:
      display.print((int)Energy);
      CurrentStep = -1; //last step, reset counter
      break;   
    default:      
      CurrentStep = -1; //nothing matched - reset counter
      break;
  }
 CurrentStep++;  
}

//***PC SPEAKER***
void OnSound(){
  tone(BuzzerOutPin,500);
  delay(100);
  noTone(BuzzerOutPin);
  tone(BuzzerOutPin,1000);
  delay(100);
  noTone(BuzzerOutPin);
} 

void OffSound(){
  tone(BuzzerOutPin,1000);
  delay(100);
  noTone(BuzzerOutPin);
  tone(BuzzerOutPin,500);
  delay(100);
  noTone(BuzzerOutPin);
} 
