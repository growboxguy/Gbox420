//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Sending email alert

//Libraries
#include "ELClientRest.h" // ESP-link - REST API

//Global variables
char WebMessage[512];

//Global constants
const char PushingBoxEmailAlertID[]  = "vC5244859A2453AA";  //UPDATE THIS to your PushingBox email alert scenario`s DeviceID 

//Component initialization
ELClient ESPLink(&Serial3);  //ESP-link. Both SLIP and debug messages are sent to ESP over Serial3
ELClientRest RestAPI(&ESPLink);    // ESP-link REST API

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //wifi console output
  ESPLink.resetCb = ResetWebServer;  //Callback subscription: When wifi is reset, restart the webServer
  ResetWebServer();
}

void loop() {  // put your main code here, to run repeatedly:
sendEmailAlert(F("Test%20alert%20title"),F("Test%20alert%20body")); //Generate URL encoded strings: https://meyerweb.com/eric/tools/dencoder/
delay(60000);
}

void ResetWebServer(void) {  
  Serial.println("WebServer (re)starting");
  while(!ESPLink.Sync())  {
    Serial.print(".");
    Serial3.print(".");
    delay(500);
    };
  RestAPI.begin("api.pushingbox.com"); //Pre-setup relay to Google Sheets 
  Serial.println("WebServer started"); 
}

void sendEmailAlert(const __FlashStringHelper *title,const __FlashStringHelper *body){
  Serial.println("Sending email alert");
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat_P(WebMessage,(PGM_P)F("/pushingbox?devid=")); strcat(WebMessage,PushingBoxEmailAlertID); 
  strcat_P(WebMessage,(PGM_P)F("&Title=")); strcat_P(WebMessage,(PGM_P)title);
  strcat_P(WebMessage,(PGM_P)F("&Log=")); strcat_P(WebMessage,(PGM_P)body); 
  Serial.println(WebMessage);   
  RestAPI.get(WebMessage);
}
