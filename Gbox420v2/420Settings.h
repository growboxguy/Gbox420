#ifndef Settings_H
#define Settings_H

static const byte MaxTextLength = 32;  //Default char* buffer size: 31 characters + null terminator, used for logging and converting to text
static const byte RollingAverageQueueDepth = 10;  //How many previous sensor readings should be stored
static const byte LogDepth = 10;  //Show X number of log entries on website, do not go above 10

typedef struct
{ 
  byte Version=7;  //UPDATE this every time you change something in the stucture, that will force the EEPROM stored settings to update.
    
  bool ATXPowerSupplyOn = true; //ATX power supply ON(true) or OFF(false)
  
  bool AeroSprayEnabled = true;  //Enable/disable misting
  uint32_t AeroInterval = 15; //Aeroponics - Spray every 15 minutes
  uint32_t AeroDuration = 2; //Aeroponics - Spray time in seconds  
  uint32_t AeroPumpTimeout = 360;  // Aeroponics - Max pump run time in seconds (6 minutes), measue zero to max pressuretank refill time and adjust accordingly
  uint32_t AeroPrimingTime = 10;  // Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.

  uint8_t InternalDHTSensorPin = 43; //DAT - DHT22 temp/humidity sensor, internally mounted
  uint8_t ExternalDHTSensorPin = 44; //DAT - DHT22 temp/humidity sensor, externally mounted

  //LightSensor1 settings
  byte LightSensor1DigitalPin = 36; //D0 - LM393 light sensor
  byte LightSensor1AnalogPin = A0; //A0 - LM393 light sensor   //A0 is just a variable that contains the actual pin number - https://github.com/arduino/ArduinoCore-avr/blob/master/variants/standard/pins_arduino.h#L56-L72
 
  //Light1 settings
  byte Light1RelayPin = 29;  //Power relay Port 8 - LED lights
  byte Light1DimmingPin = 11; //PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
  byte Light1DimmingLimit = 8;  //Blocks dimming below 8%, HLG drivers cannot fully dimm. Check the LED driver specification and adjust accordingly, only set 0 if it supports dimming fully! (Usually not the case..)
  bool Light1Status = true;  //Startup status for lights: True-ON / False-OFF
  byte Light1Brightness = 15; //Light intensity: 0 - 100 range for controlling led driver output
  bool Light1TimerEnabled = true;  //Enable timer controlling lights  
  byte Light1OnHour = 4;  //Light ON time - hour
  byte Light1OnMinute = 20; //Light ON time - minute
  byte Light1OffHour = 16; //Light OFF time - hour
  byte Light1OffMinute = 20; //Light OFF time - minute  

  bool AutomaticInternalFan = false;  //Adjust internal fan based on temperature
  bool AutomaticExhaustFan = false;  //Adjust exhaust fan based on temp and humidity
  bool InternalFanOn = true;  //Internal fan On/Off, default:ON
  bool InternalFanHigh = true; //Internal fan Low/High, default:Low
  bool ExhaustFanOn = true;  //Exhaust fan On/Off, default:OFF
  bool ExhaustFanHigh = true;  //Exhaust fan Low/High, default:Low
  int InternalFanSwitchTemp = 25; // Above limit turn the internal fan to High, turn to Low if limit-3 degrees is reached. Has to match default unit type(Metric C or Imperial K)
  byte ExhaustFanHighHumid = 65; //Above set humidity turn exhaust fan High if automatic fan control is enabled
  byte ExhaustFanLowHumid = 55; //Above set humidity turn exhaust fan Low if automatic fan control is enabled
  byte ExhaustFanOffHumid = 40; //Below set humidity turn exhaust fan Off if automatic fan control is enabled
   
  bool SoundEnabled = true;  //Enable PC speaker
  bool DebugEnabled = true; //Logs debug messages to serial and web outputs
  bool MetricSystemEnabled = true; //Swith between Imperial/Metric units. If changed update the default temp and pressure values.  
  bool ReportToGoogleSheets = true;  //Controls reporting sensor readings to Google Sheets
  bool ReportToMqtt = true;    //Controls reporting sensor readings to an MQTT broker
  char PushingBoxLogRelayID[MaxTextLength]= "v420"; //UPDATE THIS DeviceID of the PushingBox logging scenario 

  byte DigitDisplayBacklight = 25; //4 digit display - backlight strenght (0-100)
  byte DigitDisplayValue = 0; //select which sensor reading to display(1-18), 0 cycle through all values
    
  bool AlertEmails = true; //disable/enable email sending
  char PushingBoxEmailRelayID[MaxTextLength]  = "v420";  //UPDATE THIS DeviceID of the PushingBox email alert scenario
  int ReadCountBeforeAlert = 12; //number of consecutive out of range sensor readings before the email alert is triggered (5sec between reads -> 12= Out of range reading through 1 minute)  
  int TempAlertLow = 15; //Low temp warning email
  int TempAlertHigh = 35; //High temp warning email
  int HumidityAlertLow = 35; //Low humidity warning email
  int HumidityAlertHigh = 70; //High humidity warning email
  float PressureAlertLow = 4.0; //Low pressure warning
  float PressureAlertHigh = 8.0; //High pressure warning  
  float PHAlertLow = 5.5; //Low pressure warning
  float PHAlertHigh = 6.5; //High pressure warning

  float PHCalibrationSlope = -0.033256;     //Update this to your own PH meter calibration values
  float PHCalibrationIntercept = 24.08651;  //Update this to your own PH meter calibration values
  float PressureSensorOffset = 0.57;        //Pressure sensor calibration: voltage reading at 0 pressure
  float PressureSensorRatio = 2.7;          //Pressure sensor voltage to pressure ratio 

//Analog pins
  byte PressureSensorInPin = A1; //Signal(yellow) - Pressure sensor
  byte PHMeterInPin = A3;  //Po - PH meter
  byte WaterCriticalInPin = A4; //Water sensor1
  byte WaterLowInPin = A5;     //Water sensor2
  byte WaterMediumInPin = A6; //Water sensor3
  byte WaterFullInPin = A7; // Water sensor4
  
//Digital pins
  byte BuzzerOutPin = 4; //PC speaker+ (red)
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

}Settings;

#endif
