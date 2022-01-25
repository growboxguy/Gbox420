# 1 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
/*! \file 
 *  \brief     Main module for Mega2560 - Grow tent monitoring and controlling sketch.
 *  \details   To change the default pin layout / startup settings navigate to: Settings.h
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/
 *  \version   4.20
 * 
 *  \todo Proper doxygen documentation
 */

# 11 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 12 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 13 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 14 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 15 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 16 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 17 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 18 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 19 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 20 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 21 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 22 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 23 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 24 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 25 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 26 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2
# 27 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 2

// Global variable initialization
char LongMessage[MaxLongTextLength] = ""; ///< Temp storage for assembling long messages (REST API, MQTT reporting)
char ShortMessage[MaxShotTextLength] = ""; ///< Temp storage for assembling short messages (Log entries, Error messages)
char CurrentTime[MaxWordLength] = ""; ///< Buffer for storing current time in text format

// Component initialization
HardwareSerial &ArduinoSerial = Serial; ///< Reference to the Arduino Serial output
HardwareSerial &ESPSerial = Serial3; ///< Reference to the ESP Link Serial output
ELClient ESPLink(&ESPSerial); ///< ESP-link. Both SLIP and debug messages are sent to ESP over the ESP Serial link
ELClientWebServer WebServer(&ESPLink); ///< ESP-link - WebServer API
ELClientCmd ESPCmd(&ESPLink); ///< ESP-link - Helps getting the current time from the internet using NTP
ELClientRest PushingBoxRestAPI(&ESPLink); ///< ESP-link - REST API
ELClientMqtt MqttAPI(&ESPLink); ///< ESP-link - MQTT protocol for sending and receiving messages
Settings *ModuleSettings; ///< This object will store the settings loaded from the EEPROM. Persistent between reboots.
bool *Debug; ///< True - Turns on extra debug messages on the Serial Output
bool *Metric; ///< True - Use metric units, False - Use imperial units
bool MqttConnected = false; ///< Track the connection state to the MQTT broker configured on the ESP-link's REST/MQTT tab
MainModule *Main1; ///< Represents a Grow Box with all components (Lights, DHT sensors, Power sensor, Aero/Hempy/Reservoir wireless modules,..etc)

RF24 Wireless(WirelessCEPin, WirelessCSNPin); ///< Wireless communication with Modules over nRF24L01+

// Thread initialization
Thread OneSecThread = Thread();
Thread FiveSecThread = Thread();
Thread MinuteThread = Thread();
StaticThreadController<3> ThreadControl(&OneSecThread, &FiveSecThread, &MinuteThread);

void setup()
{ ///<  put your setup code here, to run once:
  ArduinoSerial.begin(115200); ///< 2560mega console output
  ESPSerial.begin(115200); ///< ESP WiFi console output
  pinMode((13), 0x1); ///< onboard LED - Heartbeat every second to confirm code is running
  printf_begin(); ///< Needed to print wireless status to Serial
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 61 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 61 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              ""
# 61 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 61 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 0); ///< New line
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 62 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 62 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "Main module initializing"
# 62 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 62 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 0); ///< logs to both Arduino and ESP serials, adds new line after the text (true), and uses no indentation (0). More on why texts are in F(""):  https://gist.github.com/sticilface/e54016485fcccd10950e93ddcd4461a3
  wdt_enable(
# 63 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
            9
# 63 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                   ); ///< Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds it assumes a lockup and resets the sketch
  
# 64 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
 (__extension__({ __asm__ __volatile__ ( "sts %0, %1\n\t" "spm\n\t" : : "i" (((uint16_t) &((*(volatile uint8_t *)((0x37) + 0x20))))), "r" ((uint8_t)(((1 << (0)) | (1 << (4))))) ); }))
# 64 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                  ; ///< fix watchdog not loading sketch after a reset error on Mega2560

  // Loading settings from EEPROM
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 67 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 67 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "Loading settings"
# 67 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 67 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 0);
  ModuleSettings = loadSettings();
  Debug = &ModuleSettings->Debug;
  Metric = &ModuleSettings->Metric;

  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 72 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 72 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "Setting up ESP-link connection"
# 72 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 72 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 0);
  ESPLink.resetCb = &resetWebServer; ///< Callback subscription: What to do when WiFi reconnects
  resetWebServer(); ///< reset the WebServer
  setSyncProvider(getNtpTime); ///< Points to method for updating time from NTP server
  setSyncInterval(86400); ///< Sync time every day
  if ((ModuleSettings->Main1).ReportToMQTT)
  {
    setupMqtt(); //MQTT message relay setup. Logs "ConnectedCB is XXXX" to serial if successful
  }
  // Threads - Setting up how often threads should be triggered and what functions to call when the trigger fires
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 82 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 82 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "Setting up refresh threads"
# 82 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 82 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), false, 0);
  OneSecThread.setInterval(1000);
  OneSecThread.onRun(runSec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(runFiveSec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(runMinute);
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 89 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 89 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "done"
# 89 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 89 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 3);

  // Start interrupts to handle request from ESP-link firmware
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 92 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 92 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "Setting up interrupt handler"
# 92 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 92 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), false, 0);
  Timer3.initialize(500); ///< check every 0.5sec, using a larger interval can cause web requests to time out
  Timer3.attachInterrupt(processTimeCriticalStuff);
  Timer3.start();
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 96 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 96 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "done"
# 96 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 96 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 3);

  //Initialize wireless communication with remote Modules
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 99 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 99 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "Setting up wireless transceiver"
# 99 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 99 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), false, 0);
  Wireless.begin(); ///< Initialize the nRF24L01+ wireless chip for talking to Modules
  Wireless.setDataRate(RF24_250KBPS); ///< Set the speed to slow - has longer range + No need for faster transmission, Other options: RF24_2MBPS, RF24_1MBPS
  Wireless.setCRCLength(RF24_CRC_16); ///< RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
  Wireless.setPALevel(RF24_PA_MAX); ///< RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, and RF24_PA_MAX=0dBm.
  Wireless.setPayloadSize(WirelessPayloadSize); ///< The number of bytes in the payload. This implementation uses a fixed payload size for all transmissions
  Wireless.enableDynamicPayloads(); ///< Required for ACK messages
  Wireless.enableAckPayload(); ///< When sending a wireless package, expect a response confirming the package was received in a custom Acknowledgement package
  Wireless.setRetries(WirelessDelay, WirelessRetry); ///< Defined in Settings.h. How many retries before giving up sending a single package and How long to wait between each retry
  Wireless.stopListening();
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 109 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 109 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "done"
# 109 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 109 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 3);

  // Create the Module objects
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 112 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 112 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "Creating Main module"
# 112 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 112 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 0);
  Main1 = new MainModule((reinterpret_cast<const __FlashStringHelper *>(
# 113 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                        (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 113 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                        "Main1"
# 113 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                        ); &__c[0];}))
# 113 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                        )), &ModuleSettings->Main1, &Wireless); ///< This is the main object representing an entire Grow Box with all components in it. Receives its name and the settings loaded from the EEPROM as parameters

  //   sendEmailAlert(F("Grow%20box%20(re)started"));
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 116 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 116 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "Setup ready, starting loops:"
# 116 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 116 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 0);
}

void loop()
{
  ThreadControl.run(); ///< loop only checks if it's time to trigger one of the threads (runSec(), runFiveSec(),runMinute()..etc)
}

void processTimeCriticalStuff()
{
  ESPLink.Process(); ///< Interrupt calls this every 0.5 sec to process any request coming from the ESP-Link hosted webpage. Uses Serial Line Internet Protocol (SLIP)
}

// Threads

void runSec()
{
  
# 133 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
 __asm__ __volatile__ ("wdr")
# 133 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
            ; ///< reset watchdog timeout
  HeartBeat(); ///< Blinks built-in led
  Main1->runSec();
}

void runFiveSec()
{
  
# 140 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
 __asm__ __volatile__ ("wdr")
# 140 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
            ;
  Main1->runFiveSec();
}

void runMinute()
{
  
# 146 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
 __asm__ __volatile__ ("wdr")
# 146 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
            ;
  Main1->runMinute();
  getWirelessStatus();
}

/**
  \brief Turns the integrated LED on the Arduino board ON/OFF 
*/
void HeartBeat()
{
  static bool ledStatus;
  ledStatus = !ledStatus;
  digitalWrite((13), ledStatus);
}

// Website related functions

/**
  \brief (re-)Initialize the ESP-link connection
*/
void resetWebServer()
{ ///<  Callback made from esp-link to notify that it has just come out of a reset
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 168 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 168 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "(re)Connecting ESP-link"
# 168 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 168 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), false, 1);
  while (!ESPLink.Sync())
  {
    logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 171 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 171 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                "."
# 171 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                ); &__c[0];}))
# 171 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                )), false, 0);
    delay(1000);
  };
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 174 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 174 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              ""
# 174 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 174 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 0); ///< line break
  if (PushingBoxRestAPI.begin("api.pushingbox.com") == 0) ///< Pre-setup relay to Google Sheets
  {
    logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 177 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 177 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                "PushingBox RestAPI ready"
# 177 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                ); &__c[0];}))
# 177 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                )), true, 2);
  }
  else
    logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 180 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 180 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                "PushingBox RestAPI failed"
# 180 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                ); &__c[0];}))
# 180 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                )), true, 2); ///< If begin returns a negative number the initialization failed
  WebServer.setup();
  URLHandler *GrowBoxHandler = WebServer.createURLHandler("/Main.html.json"); ///< setup handling request from GrowBox.html
  GrowBoxHandler->loadCb.attach(&loadCallback); ///< GrowBox tab - Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&refreshCallback); ///< GrowBox tab - Called periodically to refresh website content
  GrowBoxHandler->buttonCb.attach(&buttonCallback); ///< GrowBox tab - Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&fieldCallback); ///< GrowBox tab - Called when a field is changed on the website
  URLHandler *SettingsHandler = WebServer.createURLHandler("/Settings.html.json"); ///< setup handling request from Settings.html
  SettingsHandler->loadCb.attach(&settingsLoadCallback); ///< Settings tab - Called then the website loads initially
  SettingsHandler->refreshCb.attach(&settingsRefreshCallback); ///< Settings tab - Called periodically to refresh website content
  SettingsHandler->buttonCb.attach(&settingsButtonCallback); ///< Settings tab - Called when a button is pressed on the website
  SettingsHandler->setFieldCb.attach(&settingsFieldCallback); ///< Settings tab - Called when a field is changed on the website
  URLHandler *HempyHandler = WebServer.createURLHandler("/Hempy.html.json"); ///< setup handling request from Hempy.html (embeds the Stand-alone Hempy module's web interface)
  HempyHandler->loadCb.attach(&ignoreCallback); ///< Ignore event, handled by the Stand-alone Hempy module
  HempyHandler->refreshCb.attach(&ignoreCallback); ///< Ignore event, handled by the Stand-alone Hempy module

  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 196 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 196 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "ESP-link ready"
# 196 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 196 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), true, 1);
}

/**
  \brief Sets up the MQTT relay
*/
void setupMqtt()
{

  MqttAPI.connectedCb.attach(mqttConnected);
  MqttAPI.disconnectedCb.attach(mqttDisconnected);
  MqttAPI.publishedCb.attach(mqttPublished);
  MqttAPI.dataCb.attach(mqttReceived);

  memset(&ShortMessage[0], 0, MaxShotTextLength); //reset variable to store the Publish to path
  strcat(ShortMessage, ModuleSettings->MqttLwtTopic);
  MqttAPI.lwt(ShortMessage, ModuleSettings->MqttLwtMessage, 0, 1); //(topic,message,qos,retain) declares what message should be sent on it's behalf by the broker after Gbox420 has gone offline.
  MqttAPI.setup();
}

void mqttConnected(__attribute__((unused)) void *response)
{
  MqttAPI.subscribe(ModuleSettings->MqttSubTopic);
  MqttConnected = true;
  //if(*Debug) logToSerials(F("MQTT connected"), true);
}

void mqttDisconnected(__attribute__((unused)) void *response)
{
  //if(*Debug) logToSerials(F("MQTT disconnected"), true);
  MqttConnected = false;
}

void mqttPublished(__attribute__((unused)) void *response)
{
  //if(*Debug) logToSerials(F("MQTT published"), true);
}

void mqttReceived(void *response)
{
  static uint8_t MqttSubTopicLength = strlen(ModuleSettings->MqttSubTopic) - 1; //Get length of the command topic
  static char command[MaxShotTextLength];
  static char data[MaxShotTextLength];
  ELClientResponse *res = (ELClientResponse *)response;
  String mqttTopic = (*res).popString();
  String mqttData = (*res).popString();
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 242 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 242 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "MQTT"
# 242 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 242 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), false, 0);
  logToSerials(&mqttTopic, false, 1);
  mqttTopic.remove(0, MqttSubTopicLength); //Cut the known command topic from the arrived topic
  mqttTopic.toCharArray(command, MaxShotTextLength);
  mqttData.toCharArray(data, MaxShotTextLength);
  Main1->commandEventTrigger(command, data);
  Main1->reportToMQTTTrigger(true); //send out a fresh report
}

static bool SyncInProgress = false; ///< True if an time sync is in progress

/**
  \brief Update the time over ESP-link using NTP (Network Time Protocol)
*/
time_t getNtpTime()
{
  time_t NTPResponse = 0;
  if (!SyncInProgress)
  { // block calling the sync again inside an interrupt while already waiting for a sync to finish
    SyncInProgress = true;
    uint32_t LastRefresh = millis();
    logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 263 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 263 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                "Waiting for NTP time"
# 263 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                ); &__c[0];}))
# 263 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                )), false, 0);
    while (NTPResponse == 0 && millis() - LastRefresh < 15000)
    {
      NTPResponse = ESPCmd.GetTime();
      delay(1000);
      logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 268 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 268 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                  "."
# 268 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                  ); &__c[0];}))
# 268 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                  )), false, 0);
      
# 269 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
     __asm__ __volatile__ ("wdr")
# 269 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                ; ///reset watchdog timeout
    }
    SyncInProgress = false;
    if (NTPResponse == 0)
    {
      logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 274 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 274 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                  "sync failed"
# 274 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                  ); &__c[0];}))
# 274 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                  )), true, 3);
    }
    else
      logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 277 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 277 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                  "synchronized"
# 277 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                  ); &__c[0];}))
# 277 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                  )), true, 3);
  }
  return NTPResponse;
}

/**
  \brief Called when a website is loading on the ESP-link webserver
  \param Url - HTML filename that is getting loaded
*/
void loadCallback(__attribute__((unused)) char *Url)
{
  Main1->websiteLoadEventTrigger(Url); //Runs through all components that are subscribed to this event
}

/**
  \brief Called when a website is refreshing on the ESP-link webserver
  \param Url - HTML filename that is refreshinging
*/
void refreshCallback(__attribute__((unused)) char *Url)
{
  Main1->websiteRefreshEventTrigger(Url);
}

/**
  \brief Called when a button is pressed.
  \param Button - ID of the button HTML element
*/
void buttonCallback(char *Button)
{
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 306 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 306 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "ESP button:"
# 306 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 306 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), false, 0);
  if (strcmp_P(Button, (
# 307 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                       const char *
# 307 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                            )(reinterpret_cast<const __FlashStringHelper *>(
# 307 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                             (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 307 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                             "RestoreDef"
# 307 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                             ); &__c[0];}))
# 307 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                             ))) == 0)
  {
    restoreDefaults();
  }
  else
  {
    Main1->commandEventTrigger(Button, 
# 313 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3 4
                                      __null
# 313 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                                          );
  }
  saveSettings(ModuleSettings);
}

/**
  \brief Called when a field on the website is submitted
  \param Field - Name of the input HTML element
*/
void fieldCallback(char *Field)
{ ///< Called when any field on the website is updated.
  logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 324 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 324 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              "ESP field:"
# 324 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
              ); &__c[0];}))
# 324 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
              )), false, 0);
  Main1->commandEventTrigger(Field, WebServer.getArgString());
  saveSettings(ModuleSettings);
}

/**
  \brief Called when the /Settings.html website is loading on the ESP-link webserver
*/
void settingsLoadCallback(__attribute__((unused)) char *Url)
{
  Main1->settingsEvent_Load(Url); //Runs through all components that are subscribed to this event
}

/**
  \brief Called when the /Settings.html website is refreshing on the ESP-link webserver
*/
void settingsRefreshCallback(__attribute__((unused)) char *Url)
{
  Main1->settingsEvent_Refresh(Url);
}

/**
  \brief Called when a button is pressed on the /Settings.html web page.
  \param Button - ID of the button HTML element
*/
void settingsButtonCallback(char *Button)
{
  if (*Debug)
  {
    logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 353 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 353 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                "Settings button:"
# 353 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                ); &__c[0];}))
# 353 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                )), false, 0);
    logToSerials(&Button, true, 1);
  }
  if (strcmp_P(Button, (
# 356 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                       const char *
# 356 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                            )(reinterpret_cast<const __FlashStringHelper *>(
# 356 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                             (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 356 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                             "RestoreDef"
# 356 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                             ); &__c[0];}))
# 356 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                             ))) == 0)
  {
    restoreDefaults();
  }
  else
  {
    Main1->settingsEvent_Command(Button, 
# 362 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3 4
                                        __null
# 362 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                                            );
  }
  saveSettings(ModuleSettings);
}

/**
  \brief Called when a field on the /Settings.html website is submitted
  \param Field - Name of the input HTML element
*/
void settingsFieldCallback(char *Field)
{
  if (*Debug)
  {
    logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 375 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 375 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                "Settings field:"
# 375 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                ); &__c[0];}))
# 375 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                )), false, 0);
    logToSerials(&Field, true, 1);
  }
  Main1->settingsEvent_Command(Field, WebServer.getArgString());
  saveSettings(ModuleSettings);
}

/**
  \brief Prints the nRF24L01+ wireless transceiver's status to the Serial log
*/
void getWirelessStatus()
{
  if (*Debug)
  {
    logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 389 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 389 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                "Wireless report:"
# 389 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                ); &__c[0];}))
# 389 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                )), true, 0);
    Wireless.printPrettyDetails();
    logToSerials((reinterpret_cast<const __FlashStringHelper *>(
# 391 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 391 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                ""
# 391 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino" 3
                ); &__c[0];}))
# 391 "b:\\Gbox420\\Gbox420\\Gbox420_Mega_Main\\Gbox420_Mega_Main.ino"
                )), true, 0);
  }
}

/**
  \brief Ignores the incoming loa/refresh event. Used when embedding another module's web interface that already handles the event
*/
void ignoreCallback(__attribute__((unused)) char *Url)
{
}
