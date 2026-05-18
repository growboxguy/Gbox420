#pragma once

/*! 
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.  
 *  \warning   EEPROM has a write limit of 100,000 cycles. Constantly updating variables inside a loop will wear out the EEPROM memory! 
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 14; ///< Increment this after changing the structure of the SAVED TO EEPROM section to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
static const uint8_t MaxWordLength = 32;       ///< Default char* buffer length for storing a single word + null terminator. Memory intense!
static const uint8_t MaxShortTextLength = 64;   ///< Default char* buffer length for storing multiple words. Memory intense!
static const uint16_t MaxLongTextLength = 256; ///< Default char* buffer length for storing long text strings. Memory intense!
static const uint8_t QueueDepth = 8;           ///< Limits the maximum number of active modules. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temporary storage array for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShortTextLength]; // Temporary storage array for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing the current time string in text format

///< nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 9;              ///< nRF24l01+ wireless transmitter CSN pin - Pre-connected on RF-Nano
static const uint8_t WirelessCEPin = 10;              ///< nRF24l01+ wireless transmitter CE pin - Pre-connected on RF-Nano
static const uint8_t WirelessChannel[6] = {"Res1"};   ///< Unique channel name matching the corresponding ReservoirModule_Web object definition inside MainModule_Web.cpp
static const uint8_t WirelessPayloadSize = 32;        ///< Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
static const uint16_t WirelessMessageTimeout = 500;   ///< (ms) Complete single package exchange cycle time limit window (including retries and delays).
static const uint16_t WirelessReceiveTimeout = 60000; ///< (ms) If no wireless packages arrive from the Main module within this limit, trigger an nRF24L01+ receiver reset sequence.

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct __attribute__((packed))
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial and Metric units. If changed, manually update the default temperature and pressure values below.

  // initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
  struct ReservoirModuleSettings
  {
    ReservoirModuleSettings(bool SerialReportDate = true, bool SerialReportMemory = true, bool SerialReportJSONFriendly = true, bool SerialReportJSON = true, bool SerialReportWireless = true) : SerialReportDate(SerialReportDate), SerialReportMemory(SerialReportMemory), SerialReportJSONFriendly(SerialReportJSONFriendly), SerialReportJSON(SerialReportJSON), SerialReportWireless(SerialReportWireless) {}
    bool SerialReportDate;         ///< Enable/disable printing timestamps to the Serial monitor output
    bool SerialReportMemory;       ///< Enable/disable printing available dynamic free memory status to the Serial monitor output
    bool SerialReportJSONFriendly; ///< Enable/disable sending text formatted data streams to the Serial monitor output
    bool SerialReportJSON;         ///< Enable/disable streaming raw JSON formatted data strings to the Serial monitor output
    bool SerialReportWireless;     ///< Enable/disable outputting RF transceiver package exchange statistics to the Serial monitor
  };
  struct ReservoirModuleSettings Res1 = {.SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSONFriendly = true, .SerialReportJSON = true, .SerialReportWireless = true};

  struct DHTSensorSettings ///< DHTSensor default settings
  {
    DHTSensorSettings(uint8_t Pin = 0, uint8_t Type = 0) : Pin(Pin), Type(Type) {}
    uint8_t Pin;  ///< DAT pin of the DHT sensor
    uint8_t Type; ///< Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

  struct PHSensorSettings ///< PHSensor default settings
  {
    PHSensorSettings(uint8_t Pin = 0, float Slope = 0.0, float Intercept = 0.0) : Slope(Slope), Intercept(Intercept), Pin(Pin) {}
    float Slope;
    float Intercept;
    uint8_t Pin; ///< pH sensor Po pin
  };
  struct PHSensorSettings PHSen1 = {.Slope = -0.031199, .Intercept = 22.557617, .Pin = A0}; ///< Update this to your own PH meter calibration values: https://sites.google.com/site/growboxguy/arduino/reservoir-module?authuser=0#h.p_PGMrIJ9iRr0c

  struct TDSSensorSettings ///< PHSensor default settings
  {
    TDSSensorSettings(uint8_t Pin = 0, uint8_t PowerPin = 0) : Pin(Pin), PowerPin(PowerPin) {}
    uint8_t Pin;      ///< TDS sensor A pin
    uint8_t PowerPin; ///< TDS sensor A pin
  };
  struct TDSSensorSettings TDS1 = {.Pin = A1, .PowerPin = A2};

  struct SoundSettings ///< Sound default settings
  {
    SoundSettings(uint8_t Pin = 0) : Pin(Pin) {}
    uint8_t Pin;         ///< Piezo Buzzer red(+) cable
    bool Enabled = true; ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 2};

  struct WaterTempSensorSettings ///< WaterTempSensor default settings
  {
    WaterTempSensorSettings(uint8_t Pin = 0) : Pin(Pin) {}
    uint8_t Pin; ///< DS18B20 sensor yellow signal wire
  };
  struct WaterTempSensorSettings WTemp1 = {.Pin = 4}; ///< Data(yellow) - DS18B20 waterproof temp sensor

  struct WeightSensorSettings ///< WeightSensor default settings
  {
    WeightSensorSettings(uint8_t DTPin = 0, uint8_t SCKPin = 0, long Offset = 0, float Scale = 0.0) : Offset(Offset), Scale(Scale), DTPin(DTPin), SCKPin(SCKPin) {}
    long Offset;    ///< Reading at 0 weight on the scale
    float Scale;    ///< Scale factor
    uint8_t DTPin;  ///< Weight sensor DT pin
    uint8_t SCKPin; ///< Weight sensor SCK pin
  };
  struct WeightSensorSettings Weight1 = {.Offset = 252769, .Scale = -21033.98, .DTPin = 5, .SCKPin = 6};    ///< Update the calibration values here for the Weight Sensor
  struct WeightSensorSettings WeightWR = {.Offset = 260682, .Scale = -22084.60, .DTPin = A3, .SCKPin = A4}; ///< Waste Reservoir Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_HempyWastePlatforms/Test-WeightSensor_HempyWastePlatforms.ino

  uint8_t CompatibilityVersion = Version; ///< Structural layout validation indicator. This must strictly remain the terminal variable definition located inside the struct memory footprint.
} Settings;

/**
  \brief Store settings in EEPROM - Only updates changed bits
  \attention Use cautiously, EEPROM has a write limit of 100,000 cycles
*/
void saveSettings(Settings *ToSave);
/**
  \brief Load settings from EEPROM
  \param ResetEEPROM - Force loading the defaults from the sketch and overwriting the EEPROM with it
  \return Reference to Settings object
*/
Settings *loadSettings(bool ResetEEPROM = false);
void restoreDefaults();