//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Reporting to Google Sheets using ESP-Link REST API
//More details: https://sites.google.com/site/growboxguy/arduino/logging
//Update the SheetsRelayDeviceID constant before uploading!

//Libraries
#include "ELClientRest.h" // ESP-link - REST API

//Global constants
const char PushingBoxLogRelayID[] = "v755877CF53383E1"; //UPDATE THIS to your PushingBox logging scenario`s DeviceID

//Global variables - fake data to report
char WebMessage[512]; //buffer for GoogleSheets report

// Component initialization
ELClient ESPLink(&Serial3);     //ESP-link. Both SLIP and debug messages are sent to ESP over Serial3
ELClientRest RestAPI(&ESPLink); // ESP-link REST API

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);             //2560mega console output
  Serial3.begin(115200);            //WiFi console output
  ESPLink.resetCb = ResetWebServer; //Callback subscription: When WiFi is reset, restart the webServer
  ResetWebServer();
}

void loop()
{ // put your main code here, to run repeatedly:
  reportToGoogleSheets();
  delay(60000);
}

void ResetWebServer(void)
{
  Serial.println(F("Sketch for testing: Reporting to Google Sheets using ESP-Link REST API"));
  Serial.println();
  Serial.println(F("WebServer (re)starting"));
  while (!ESPLink.Sync())
  {
    Serial.print(F("."));
    Serial3.print(F("."));
    delay(500);
  };
  RestAPI.begin("api.pushingbox.com"); //Pre-setup relay to Google Sheets
  Serial.println(F("WebServer started"));
}

void reportToGoogleSheets()
{
  Serial.println(F("Reporting to Google Sheets"));
  memset(&WebMessage[0], 0, sizeof(WebMessage)); //clear variable
  strcat(WebMessage, "/pushingbox?devid=");
  strcat(WebMessage, PushingBoxLogRelayID);
  strcat_P(WebMessage, (PGM_P)F("&BoxData="));
  strcat_P(WebMessage, (PGM_P)F("{\"Log\":{\"IFan\":{\"S\":\"1\"},\"EFan\":{\"S\":\"1\"},\"Lt1\":{\"S\":\"1\",\"B\":\"15\",\"T\":\"1\",\"On\":\"04:20\",\"Of\":\"16:20\"}}}"));
  Serial.println(WebMessage);
  RestAPI.get(WebMessage);
}

//URL called by the script: http://api.pushingbox.com/pushingbox?devid=v755877CF53383E1&BoxData={"Log":{"IFan":{"S":"1"},"EFan":{"S":"2"},"Lt1":{"S":"1","B":"42","T":"1","On":"04:20","Of":"16:20"}}}