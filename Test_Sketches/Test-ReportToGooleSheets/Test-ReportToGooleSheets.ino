//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Reporting to Google Sheets using ESP-Link REST API
//Update the SheetsRelayDeviceID constant before uploading!

//Libraries
#include "ELClientRest.h" // ESP-link - REST API

//Global constants
const char PushingBoxLogRelayID[]= "v420"; //UPDATE THIS to your PushingBox logging scenario`s DeviceID 

//Global variables - fake data to report
char WebMessage[512];   //buffer for GoogleSheets report
float Humidity = 4.20; 
bool isLightOn = true;
int Brightness =80;
char Message[] = "GrowBoxGuy";

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
reportToGoogleSheets();
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

void reportToGoogleSheets(){
  Serial.println("Reporting to Google Sheets");
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat(WebMessage,"/pushingbox?devid="); strcat(WebMessage,PushingBoxLogRelayID);
  strcat_P(WebMessage,(PGM_P)F("&Log="));
  strcat_P(WebMessage,(PGM_P)F("{\"Humidity\":\""));  strcat(WebMessage,floatToChar(Humidity));
  strcat_P(WebMessage,(PGM_P)F("\",\"isLightOn\":\""));  strcat(WebMessage,intToChar(isLightOn));
  strcat_P(WebMessage,(PGM_P)F("\",\"Brightness\":\""));  strcat(WebMessage,intToChar(Brightness));
  strcat_P(WebMessage,(PGM_P)F("\",\"Message\":\""));  strcat(WebMessage,Message);  
  strcat_P(WebMessage,(PGM_P)F("\"}"));
  Serial.println(WebMessage);   
  RestAPI.get(WebMessage);
} //api.pushingbox.com/pushingbox?devid=v420&Log={"Humidity":"4.20","isLightOn":"1","Brightness":"80","Message":"GrowBoxGuy"}

char * intToChar(int Number){
static char ReturnChar[8] = ""; //7 digits + null terminator
itoa(Number, ReturnChar, 10);
return ReturnChar;
}

char * floatToChar(float Number){  //4 digits + decimal sign + 2 decimals + null terminator
static char ReturnFloatChar[8] = "";
dtostrf(Number, 4, 2, ReturnFloatChar); 
return ReturnFloatChar;
}
