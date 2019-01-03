void readSensors(){  //Sensor readings  
  Humidity = TempSensor.readHumidity();
  BoxTempC= TempSensor.readTemperature();
  BoxTempF = BoxTempC *1.8 + 32;
  checkBrightness();
  Voltage = PowerSensor.voltage(PowerSensorIP);
  Current = PowerSensor.current(PowerSensorIP);
  Power = PowerSensor.power(PowerSensorIP);
  Energy = PowerSensor.energy(PowerSensorIP) / 1000;  //total power consumption in kWh
  readPH();
  readMoisture();
  checkReservoir();
  readAeroPressure();
}

//***PH METER***
void readPH(){
  float  Reading=0;
  for(byte i=0;i<20;i++) { 
   Reading+=analogRead(PHMeterInPin);
   delay(50);
  }
  Reading = Reading /20; //Calculates average
  //Serial.print("Analog reading: "); Serial.println(Reading);
  PH = -0.031223*Reading + 23.376812;  //equation of the line
}

//***Moisture sensor***
void readMoisture(){
  Moisture= analogRead(AMoistureSensorInPin);
  isWet = !digitalRead(DMoistureSensorInPin);
  if(Moisture > MaxMoisture) MaxMoisture = Moisture;
  if(Moisture < MinMoisture) MinMoisture = Moisture;
  MoisturePercentage = map(Moisture,MaxMoisture,MinMoisture,100,0);
}
