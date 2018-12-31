void readSensors(){  //Sensor readings  
  Humidity = TempSensor.readHumidity();
  BoxTempC= TempSensor.readTemperature();
  BoxTempF = BoxTempC *1.8 + 32;
  isBright = !digitalRead(LightSensorInPin);     // read the input pin: 0- light detected , 1 - no light detected. Had to invert signal from the sensor to make more sense.
  LightReading = 1023 - analogRead(LightSensorAnalogInPin);
  LightReadingPercent = map(LightReading,MinLightReading,MaxLightReading,0,100);
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
