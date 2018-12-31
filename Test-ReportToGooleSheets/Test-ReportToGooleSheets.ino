//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Reporting to Google Sheets using ESP-Link REST API

//Libraries
#include "ELClientRest.h" // ESP-link - REST API

//Global variables - fake data to report
char WebMessage[512];   //buffer for GoogleSheets report
float Humidity = 4.20; 
bool isLightOn = true;
int Brightness =80;
char Message[] = "GrowBoxGuy";

//Global constants
const char SheetsRelayDeviceID[] = "v420";  //Update this to your Google Sheets scenario DeviceID

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
reportToGoogleSheets();
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

void reportToGoogleSheets(){
  Serial.println("Reporting to Google Sheets");
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat(WebMessage,"/pushingbox?devid="); strcat(WebMessage,SheetsRelayDeviceID);
  strcat(WebMessage,"&Humidity=");  strcat(WebMessage,floatToChar(Humidity));  
  strcat(WebMessage,"&isLightOn=");  strcat(WebMessage,intToChar(isLightOn));
  strcat(WebMessage,"&Brightness=");  strcat(WebMessage,intToChar(Brightness));  
  strcat(WebMessage,"&Message=");  strcat(WebMessage,Message); 
  Serial.println(WebMessage);   
  rest.get(WebMessage);
} //api.pushingbox.com/pushingbox?devid=v420&Humidity=4.20&isLightOn=1&Brightness=80&Message=GrowBoxGuy

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
