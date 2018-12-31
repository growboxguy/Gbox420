//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Sending email alert

//Libraries
#include "ELClientRest.h" // ESP-link - REST API

//Global variables
char WebMessage[512];

//Global constants
const char EmailAlertDeviceID[]  = "vC5244859A2453AA";  //Update this to your email alert scenario DeviceID

//Component initialization
ELClient esp(&Serial3);  //ESP-link. Both SLIP and debug messages are sent to ESP over Serial3
ELClientRest rest(&esp);    // ESP-link REST API

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //wifi console output
  esp.resetCb = ResetWebServer;  //Callback subscription: When wifi is reset, restart the webServer
  ResetWebServer();
}

void loop() {  // put your main code here, to run repeatedly:
sendEmailAlert(F("Test%20alert%20title"),F("Go%20fuck%20yourself!%23%24%25%5E%26*("));
delay(60000);
}

void ResetWebServer(void) {  
  Serial.println("WebServer (re)starting");
  while(!esp.Sync())  {
    Serial.print(".");
    Serial3.print(".");
    delay(500);
    };
  rest.begin("api.pushingbox.com"); //Pre-setup relay to Google Sheets 
  Serial.println("WebServer started"); 
}

void sendEmailAlert(const __FlashStringHelper *title,const __FlashStringHelper *alert){
  Serial.println("Sending email alert");
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat_P(WebMessage,(PGM_P)F("/pushingbox?devid=")); strcat(WebMessage,EmailAlertDeviceID); 
  strcat_P(WebMessage,(PGM_P)F("&Title=")); strcat_P(WebMessage,(PGM_P)title);
  strcat_P(WebMessage,(PGM_P)F("&Alert=")); strcat_P(WebMessage,(PGM_P)alert); 
  Serial.println(WebMessage);   
  rest.get(WebMessage);
}
