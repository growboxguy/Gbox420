//Pins for connecting components
//https://sites.google.com/site/growboxguy/arduino/components

  const byte BuzzerOutPin = 4; //PC speaker+ (red)
  const byte BuiltInLEDOutPin = 13;  //Built-in LED light for testing
  const byte PowerLEDOutPin = 46;  //PC case Power LED
  const byte PowerButtonInPin = 49;  //Power button 
  const byte LightSensorInPin = 36; //D0 - LM393 light sensor                                                                                                              
  const byte LightSensorAnalogInPin = A0; //A0 - LM393 light sensor
  const byte WaterCriticalInPin = A4; //Water sensor1
  const byte WaterLowInPin = A5;     //Water sensor2
  const byte WaterMediumInPin = A6; //Water sensor3
  const byte WaterFullInPin = A7; // Water sensor4
  const byte DigitDisplayCLKOutPin = 31; //CLK - 4 digit LED display
  const byte DigitDisplayDI0OutPin = 30; //DIO - 4 digit LED display
  const byte TempSensorInPin = 43; //DAT - DHT22 temp/humidity sensor
  const byte Relay1OutPin = 22;  //Power relay Port 1 - Aeroponics solenoid
  const byte Relay2OutPin = 23;  //Power relay Port 2 - Aeroponics high pressure pump
  const byte Relay3OutPin = 24;  //Power relay Port 3 - PC power supply
  const byte Relay4OutPin = 25;  //Power relay Port 4 - Internal fan Off/On
  const byte Relay5OutPin = 26;  //Power relay Port 5 - Internal fan Low/High
  const byte Relay6OutPin = 27;  //Power relay Port 6 - Exhauset fan Off/On
  const byte Relay7OutPin = 28;  //Power relay Port 7 - Exhauset fan Low/High
  const byte Relay8OutPin = 29;  //Power relay Port 8 - LED lights
  const byte PHMeterInPin = A3;  //Po - PH meter
  const byte PotCSOutPin = 34;  //CS - X9C104 digital potentiometer  
  const byte PotUDOutPin = 32;  //UD - X9C104 digital potentiometer
  const byte PotINCOutPin = 33; //INC - X9C104 digital potentiometer
  const byte ScreenReset = 37; //RESET(3.3V) - Screen Screen
  const byte ScreenSCK = 38;  //SCK(3.3V) - Screen Screen
  const byte ScreenMOSI = 39; //SDO/MOSI(3.3V) - Screen Screen
  const byte ScreenMISO = 40; //SDI/MISO(3.3V), not used - Screen Screen 
  const byte ScreenCS = 41;  //CS(3.3V) - Screen Screen
  const byte ScreenDC = 42; //DC/RS(3.3V) - Screen Screen
  const byte PressureSensorInPin = A1; //Signal(yellow) - Pressure sensor
