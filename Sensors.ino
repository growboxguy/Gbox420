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
  ReadPH();
  ReadMoisture();
  isWaterAboveCritical = !digitalRead(WaterCriticalInPin);
  isWaterAboveLow = !digitalRead(WaterLowInPin);
  isWaterAboveMedium = !digitalRead(WaterMediumInPin);
  isWaterAboveFull = !digitalRead(WaterFullInPin);
  readAeroPressure();
}

//***PH METER***
void ReadPH(){//uncalibrated code
  float  Reading=0;
  for(byte i=0;i<20;i++) { 
   Reading+=analogRead(PHMeterInPin);
   delay(50);
  }
  Reading = Reading /20; //Calculates average
  Serial.print("Analog reading: "); Serial.println(Reading);
  PH = -0.031308*Reading + 22.612698;  //equation of the line
}

//***Moisture sensor***
void ReadMoisture(){
  Moisture= analogRead(AMoistureSensorInPin);
  isWet = !digitalRead(DMoistureSensorInPin);
  if(Moisture > MaxMoisture) MaxMoisture = Moisture;
  if(Moisture < MinMoisture) MinMoisture = Moisture;
  MoisturePercentage = map(Moisture,MaxMoisture,MinMoisture,100,0);
}
