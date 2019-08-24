#pragma once


//DHTSensor pins
  uint8_t InternalDHTSensorPin = 43; //DAT - DHT22 temp/humidity sensor, internally mounted
  uint8_t ExternalDHTSensorPin = 44; //DAT - DHT22 temp/humidity sensor, externally mounted

//LightSensor pins
  byte LightSensor1DigitalPin = 36; //D0 - LM393 light sensor
  byte LightSensor1AnalogPin = A0; //A0 - LM393 light sensor   //A0 is just a variable that contains the actual pin number - https://github.com/arduino/ArduinoCore-avr/blob/master/variants/standard/pins_arduino.h#L56-L72

//Lights pins
 byte Light1RelayPin = 29;  //Power relay Port 8 - LED lights
 byte Light1DimmingPin = 11; //PWM based dimming, connected to optocoupler`s base over 1k ohm resistor

//Sound pins
  byte Sound1Pin = 4; //PC speaker+ (red)

//PHSensor pins
  byte PHSensorInPin = A3;  //PH meter - Po port


//Analog pins
  byte PressureSensorInPin = A1; //Signal(yellow) - Pressure sensor
  byte WaterCriticalInPin = A4; //Water sensor1
  byte WaterLowInPin = A5;     //Water sensor2
  byte WaterMediumInPin = A6; //Water sensor3
  byte WaterFullInPin = A7; // Water sensor4
  
//Digital pins

  byte BuiltInLEDOutPin = 13;  //Built-in LED light for testing
  byte Relay1OutPin = 22;  //Power relay Port 1 - FREE
  byte Relay2OutPin = 23;  //Power relay Port 2 - Aeroponics high pressure pump
  byte Relay3OutPin = 24;  //Power relay Port 3 - Aeroponics bypass solenoid
  byte Relay4OutPin = 25;  //Power relay Port 4 - Internal fan Off/On
  byte Relay5OutPin = 26;  //Power relay Port 5 - Internal fan Low/High
  byte Relay6OutPin = 27;  //Power relay Port 6 - Exhaust fan Off/On
  byte Relay7OutPin = 28;  //Power relay Port 7 - Exhaust fan Low/High
  byte ATXPowerONOutPin = 34; //Turns ATX power supply on by connecting ATX PowerON pin to GND through an optocupler
  byte ATXPowerGoodInPin = 35; //5V signal from ATX powersupply, inverted by optocoupler: LOW if DC power output is OK
  byte ReservoirTempSensorInPin = 45;  //Data(yellow) - DS18B20 waterproof temp sensor 
