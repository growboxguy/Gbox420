//Pins for connecting components
//https://sites.google.com/site/growboxguy/arduino/components

 //Analog pins
  const byte LightSensorAnalogInPin = A0; //A0 - LM393 light sensor
  const byte PressureSensorInPin = A1; //Signal(yellow) - Pressure sensor
  const byte PHMeterInPin = A3;  //Po - PH meter
  const byte WaterCriticalInPin = A4; //Water sensor1
  const byte WaterLowInPin = A5;     //Water sensor2
  const byte WaterMediumInPin = A6; //Water sensor3
  const byte WaterFullInPin = A7; // Water sensor4
  
 //Digital pins
  const byte BuzzerOutPin = 4; //PC speaker+ (red)
  const byte DimmingOutPin = 11; //PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
  const byte BuiltInLEDOutPin = 13;  //Built-in LED light for testing
  const byte Relay1OutPin = 22;  //Power relay Port 1 - FREE
  const byte Relay2OutPin = 23;  //Power relay Port 2 - Aeroponics high pressure pump
  const byte Relay3OutPin = 24;  //Power relay Port 3 - Aeroponics bypass solenoid
  const byte Relay4OutPin = 25;  //Power relay Port 4 - Internal fan Off/On
  const byte Relay5OutPin = 26;  //Power relay Port 5 - Internal fan Low/High
  const byte Relay6OutPin = 27;  //Power relay Port 6 - Exhaust fan Off/On
  const byte Relay7OutPin = 28;  //Power relay Port 7 - Exhaust fan Low/High
  const byte Relay8OutPin = 29;  //Power relay Port 8 - LED lights
  const byte DigitDisplayCLKOutPin = 31; //CLK - 4 digit LED display
  const byte DigitDisplayDI0OutPin = 30; //DIO - 4 digit LED display
  const byte ATXPowerONOutPin = 34; //Turns ATX power supply on by connecting ATX PowerON pin to GND through an optocupler
  const byte ATXPowerGoodInPin = 35; //5V signal from ATX powersupply, inverted by optocoupler: LOW if DC power output is OK
  const byte LightSensorInPin = 36; //D0 - LM393 light sensor
  const byte ScreenReset = 37; //RESET(3.3V) - Screen Screen
  const byte ScreenSCK = 38;  //SCK(3.3V) - Screen Screen
  const byte ScreenMOSI = 39; //SDO/MOSI(3.3V) - Screen Screen
  const byte ScreenMISO = 40; //SDI/MISO(3.3V), not needed - Screen Screen 
  const byte ScreenCS = 41;  //CS(3.3V) - Screen Screen
  const byte ScreenDC = 42; //DC/RS(3.3V) - Screen Screen
  const byte InternalDHTSensorInPin = 43; //DAT - DHT22 temp/humidity sensor, internally mounted
  const byte ExternalDHTSensorInPin = 44; //DAT - DHT22 temp/humidity sensor, externally mounted
  const byte ReservoirTempSensorInPin = 45;  //Data(yellow) - DS18B20 waterproof temp sensor 
  const byte PowerLEDOutPin = 46;  //PC case Power LED
  const byte PowerButtonInPin = 48;  //Power button
