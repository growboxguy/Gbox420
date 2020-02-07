#pragma once

//Update the Version when you make change to the structure of the EEPROM stored Settings struct. This will overwrite the EEPROM settings with the sketch defaults.
static const byte Version = 1;

//THIS SECTION DOES NOT GET STORED IN EEPROM:
//Global constants
static const byte MaxTextLength = 32;      //Default char * buffer for storing a word + null terminator. Memory intense!
static const byte MaxShotTextLength = 128; //Default char * buffer for storing mutiple words. Memory intense!
static const int MaxLongTextLength = 1024; //Default char * buffer for storing a long text. Memory intense!

static const byte RollingAverageQueueDepth = 10; //How many previous sensor readings should be stored
static const byte LogDepth = 5;                  //Show X number of log entries on website. Be careful, Max 1024bytes can be passed during a Website refresh event, incuding all parameters passed
static const byte QueueDepth = 64;               //Limits the maximum number of active modules. Memory intense!
//Global variables
extern char LongMessage[MaxLongTextLength];  //temp storage for assembling long messages (REST API, MQTT API)
extern char ShortMessage[MaxShotTextLength]; //temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxTextLength];      //buffer for storing current time in text

//SAVED TO EEPROM - Settings struct
//If you change things here, increase the Version variable in line 4
typedef struct
{
  bool DebugEnabled = true;          //Logs debug messages to serial and web outputs
  bool MetricSystemEnabled = true;   //Switch between Imperial/Metric units. If changed update the default temp and pressure values too.
  bool ReportToGoogleSheets = true;  //Enable/disable reporting sensor readings to Google Sheets
  int SheetsReportingFrequency = 30; //How often to report to Google Sheets. Use 15 minute increments only! Min 15min, Max 1440 (1day)
  //bool ReportToMqtt = true;    //Controls reporting sensor readings to an MQTT broker
  char PushingBoxLogRelayID[MaxTextLength] = "v755877CF53383E1";   //UPDATE THIS DeviceID of the PushingBox logging scenario
  char PushingBoxEmailRelayID[MaxTextLength] = "vC5244859A2453AA"; //UPDATE THIS DeviceID of the PushingBox email alert scenario

  struct DHTSensorSettings
  { //initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
    DHTSensorSettings(byte Pin = 0, byte Type = 0) : Pin(Pin), Type(Type) {}
    byte Pin;
    byte Type; //Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings InDHT = {.Pin = 49, .Type = 22};
  struct DHTSensorSettings ExDHT = {.Pin = 48, .Type = 22};

  byte CompatibilityVersion = Version; //Should always be the last value stored.
} Settings;
