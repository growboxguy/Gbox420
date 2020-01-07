#include "GrowBox.h"
#include "DHTSensor.h"
#include "Lights.h"
#include "Sound.h"
#include "Fan.h"
#include "PowerSensor.h" 
//#include "PowerSensorV3.h"  //Only for PZEM004T V3.0
#include "LightSensor.h"
#include "PHSensor.h" 
#include "LightSensor.h"
#include "PressureSensor.h" 
#include "Aeroponics_Tank.h" 
#include "Aeroponics_NoTank.h" 
#include "WaterTempSensor.h"
#include "WaterLevelSensor.h"

static char Logs[LogDepth][MaxTextLength];  //two dimensional array for storing log histroy displayed on the website (array of char arrays)

GrowBox::GrowBox(const __FlashStringHelper * Name, Settings *BoxSettings): Common(Name) { //Constructor
  this -> BoxSettings = BoxSettings;
  logToSerials(F(" "),true,0); //add a breakrow to console log
  Sound1 = new Sound(F("Sound1"), this, &BoxSettings -> Sound1);
  InFan = new Fan(F("InFan"), this, &BoxSettings -> InFan);
  ExFan = new Fan(F("ExFan"), this, &BoxSettings -> ExFan);
  Light1 = new Lights(F("Light1"), this, &BoxSettings -> Light1);   //Passing BoxSettings members as references: Changes get written back to BoxSettings and saved to EEPROM. (byte *)(((byte *)&BoxSettings) + offsetof(Settings, LightOnHour))
  LightSensor1 = new LightSensor(F("LightSensor1"), this, &BoxSettings -> LightSensor1);
  Power1 = new PowerSensor(F("Power1"), this, &Serial2);
  //Power1 = new PowerSensorV3(F("Power1"), this, &Serial2); //Only for PZEM004T V3.0
  InDHT = new DHTSensor(F("InDHT"), this, &BoxSettings -> InDHT);  //passing: Component name, GrowBox object the component belongs to, Default settings)
  ExDHT = new DHTSensor(F("ExDHT"), this, &BoxSettings -> ExDHT);  //passing: Component name, GrowBox object the component belongs to, Default settings)
  //Aero_T1 = new Aeroponics_Tank(F("Aero_T1"), this, &BoxSettings ->Aero_T1_Common, &BoxSettings -> Aero_T1_Specific);
  Aero_NT1 = new Aeroponics_NoTank(F("Aero_NT1"), this, &BoxSettings -> Aero_NT1_Common, &BoxSettings -> Aero_NT1_Specific);
  Pressure1 = new PressureSensor(F("Pressure1"),this,&BoxSettings -> Pressure1);
  PHSensor1 = new PHSensor(F("PHSensor1"),this, &BoxSettings -> PHSensor1);  
  WaterTemp1 = new WaterTempSensor(F("WaterTemp1"),this,&BoxSettings -> WaterTemp1);
  WaterLevel1 = new WaterLevelSensor(F("WaterLevel1"),this,&BoxSettings -> WaterLevel1);
  AddToRefreshQueue_FiveSec(this);  //Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method 
  AddToRefreshQueue_HalfHour(this);  //Subscribing to the 30 minutes refresh queue: Calls the refresh_HalfHour() method 
  AddToWebsiteQueue_Load(this); //Subscribing to the Website load event
  AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event
  AddToWebsiteQueue_Field(this); //Subscribing to the Website field submit event
  AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event
  logToSerials(F("GrowBox object created"), true,2);
  addToLog(F("GrowBox initialized"),0);
}

void GrowBox::websiteEvent_Refresh(__attribute__((unused)) char * url) //called when website is refreshed.
{ 
  WebServer.setArgString(F("Time"), getFormattedTime());
  WebServer.setArgJson(F("list_SerialLog"), eventLogToJSON(false)); //Last events that happened in JSON format  
}

void GrowBox::websiteEvent_Load(__attribute__((unused)) char * url){ //When the website is opened, load stuff once
  if (strcmp(url,"/Settings.html.json")==0){  
  WebServer.setArgInt(getWebsiteComponentName(F("DebugEnabled")), GBox -> BoxSettings -> DebugEnabled);
  WebServer.setArgInt(getWebsiteComponentName(F("MetricSystemEnabled")), GBox -> BoxSettings -> MetricSystemEnabled);
  //WebServer.setArgBoolean(getWebsiteComponentName(F("MqttEnabled")), GBox -> BoxSettings -> ReportToMqtt);
  WebServer.setArgBoolean(getWebsiteComponentName(F("GoogleSheetsEnabled")), GBox -> BoxSettings -> ReportToGoogleSheets);
  WebServer.setArgString(getWebsiteComponentName(F("PushingBoxLogRelayID")), GBox -> BoxSettings -> PushingBoxLogRelayID);  
  //WebServer.setArgString(F("PushingBoxEmailRelayID"), GBox -> BoxSettings -> PushingBoxEmailRelayID);
  }
} 

void GrowBox::websiteEvent_Field(char * Field){ //When the website field is submitted
  if(!isThisMyComponent(Field)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name   
    if(strcmp_P(ShortMessage,(PGM_P)F("DebugEnabled"))==0) {setDebugOnOff(WebServer.getArgBoolean());}
    else if(strcmp_P(ShortMessage,(PGM_P)F("MetricSystemEnabled"))==0) {setMetricSystemEnabled(WebServer.getArgBoolean());}
    //else if(strcmp_P(ShortMessage,(PGM_P)F("MqttEnabled"))==0) {setReportToMqttOnOff(WebServer.getArgBoolean());}
    else if(strcmp_P(ShortMessage,(PGM_P)F("GoogleSheetsEnabled"))==0) {setReportToGoogleSheetsOnOff(WebServer.getArgBoolean());}
    else if(strcmp_P(ShortMessage,(PGM_P)F("PushingBoxLogRelayID"))==0) {setPushingBoxLogRelayID(WebServer.getArgString());}    
  }  
}

void GrowBox::websiteEvent_Button(char * Button){ //When a button is pressed on the website
  if(!isThisMyComponent(Button)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name   
    if(strcmp_P(ShortMessage,(PGM_P)F("GoogleSheetsTrigger"))==0) {ReportToGoogleSheets(true);}
    else if(strcmp_P(ShortMessage,(PGM_P)F("ReportTrigger"))==0) {runReport();} 
    else if(strcmp_P(ShortMessage,(PGM_P)F("Refresh"))==0) {refreshRequest = true;}   //Website signals to refresh all sensor readings
  }  
}  

void GrowBox::refresh_FiveSec(){
  if(BoxSettings -> DebugEnabled) Common::refresh_FiveSec();
  if(refreshRequest) {
    refreshRequest = false;
    refreshAll(true);
  }
}

void GrowBox::refresh_HalfHour(){
  if(BoxSettings -> DebugEnabled) Common::refresh_HalfHour();
  ReportToGoogleSheets(false);
}

void GrowBox::refreshAll(bool AddToLog){  //implementing the virtual refresh function from Common
  if(AddToLog)addToLog(F("Refresh triggered"),false);  
  //trigger all threads at startup
  runSec();
  runFiveSec(); 
  runMinute();
  runHalfHour();
  runReport();
}

void GrowBox::runReport(){ 
  getFormattedTime();  //fills the CurrentTime global variable
  logToSerials(&CurrentTime, true,0);
  getFreeMemory();  
  for(int i=0;i<reportQueueItemCount;i++){
   ReportQueue[i] -> report();
  }
}

//////////////////////////////////////////////////////////////////
//Run queues: Refresh components inside the GrowBox

void GrowBox::runSec(){ 
  if(BoxSettings -> DebugEnabled)logToSerials(F("One sec trigger.."),true,1);  
  for(int i=0;i<refreshQueueItemCount_Sec;i++){
   RefreshQueue_Sec[i] -> refresh_Sec();
  }
}

void GrowBox::runFiveSec(){
  if(BoxSettings -> DebugEnabled)logToSerials(F("Five sec trigger.."),true,1); 
  for(int i=0;i<refreshQueueItemCount_FiveSec;i++){
    RefreshQueue_FiveSec[i] -> refresh_FiveSec();
  }
}

void GrowBox::runMinute(){
  if(BoxSettings -> DebugEnabled)logToSerials(F("Minute trigger.."),true,1);
  for(int i=0;i<refreshQueueItemCount_Minute;i++){
    RefreshQueue_Minute[i] -> refresh_Minute();
  }
}

void GrowBox::runHalfHour(){   
  if(BoxSettings -> DebugEnabled)logToSerials(F("Half hour trigger.."),true,1);
  for(int i=0;i<refreshQueueItemCount_HalfHour;i++){
    RefreshQueue_HalfHour[i] -> refresh_HalfHour();
  } 
}

//////////////////////////////////////////////////////////////////
//Thread Queues: Called based on thread scheduler

void GrowBox::AddToReportQueue(Common* Component){
  if(QueueDepth>reportQueueItemCount) ReportQueue[reportQueueItemCount++] = Component;
  else logToSerials(F("Report queue overflow!"),true,0); //Too many components are added to the queue, increase "QueueDepth" variable in 420Settings.h , or shift components to a different queue 
}

void GrowBox::AddToRefreshQueue_Sec(Common* Component){
  if(QueueDepth>refreshQueueItemCount_Sec) RefreshQueue_Sec[refreshQueueItemCount_Sec++] = Component;
  else logToSerials(F("RefreshQueue_Sec overflow!"),true,0);
}

void GrowBox::AddToRefreshQueue_FiveSec(Common* Component){
  if(QueueDepth>refreshQueueItemCount_FiveSec) RefreshQueue_FiveSec[refreshQueueItemCount_FiveSec++] = Component;
  else logToSerials(F("RefreshQueue_FiveSec overflow!"),true,0);
}

void GrowBox::AddToRefreshQueue_Minute(Common* Component){
  if(QueueDepth>refreshQueueItemCount_Minute) RefreshQueue_Minute[refreshQueueItemCount_Minute++] = Component;
  else logToSerials(F("RefreshQueue_Minute overflow!"),true,0);
}

void GrowBox::AddToRefreshQueue_HalfHour(Common* Component){
  if(QueueDepth>refreshQueueItemCount_HalfHour) RefreshQueue_HalfHour[refreshQueueItemCount_HalfHour++] = Component;
  else logToSerials(F("RefreshQueue_HalfHour overflow!"),true,0);
}

//////////////////////////////////////////////////////////////////
//Website queues: Called based on Website events on the ESP-link

void GrowBox::AddToWebsiteQueue_Load(Common* Component){
  if(QueueDepth>WebsiteQueueItemCount_Load) WebsiteQueue_Load[WebsiteQueueItemCount_Load++] = Component;
  else logToSerials(F("WebsiteQueueItemCount_Load overflow!"),true,0);
}

void GrowBox::AddToWebsiteQueue_Refresh(Common* Component){
  if(QueueDepth>WebsiteQueueItemCount_Refresh) WebsiteQueue_Refresh[WebsiteQueueItemCount_Refresh++] = Component;
  else logToSerials(F("WebsiteQueueItemCount_Refresh overflow!"),true,0);
}

void GrowBox::AddToWebsiteQueue_Button(Common* Component){
  if(QueueDepth>WebsiteQueueItemCount_Button) WebsiteQueue_Button[WebsiteQueueItemCount_Button++] = Component;
  else logToSerials(F("WebsiteQueueItemCount_Button overflow!"),true,0);
}

void GrowBox::AddToWebsiteQueue_Field(Common* Component){
  if(QueueDepth>WebsiteQueueItemCount_Field) WebsiteQueue_Field[WebsiteQueueItemCount_Field++] = Component;
  else logToSerials(F("WebsiteQueueItemCount_Field overflow!"),true,0);
}

//////////////////////////////////////////////////////////////////
//Even logs on the website
void GrowBox::addToLog(const char *LongMessage,byte Indent){  //adds a log entry that is displayed on the web interface
  logToSerials(LongMessage,true,Indent);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],MaxTextLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy(Logs[0],LongMessage,MaxTextLength);  //instert new log to [0]
}

void GrowBox::addToLog(const __FlashStringHelper *LongMessage,byte Indent){ //function overloading: same function name, different parameter type 
  logToSerials(LongMessage,true,Indent);
  for(byte i=LogDepth-1;i>0;i--){   //Shift every log entry one up, dropping the oldest
     memset(&Logs[i], 0, sizeof(Logs[i]));  //clear variable
     strncpy(Logs[i],Logs[i-1],MaxTextLength ) ; 
    }  
  memset(&Logs[0], 0, sizeof(Logs[0]));  //clear variable
  strncpy_P(Logs[0],(PGM_P)LongMessage,MaxTextLength);  //instert new log to [0]
}

char * GrowBox::eventLogToJSON(bool Append){ //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  if(!Append)memset(&LongMessage[0], 0, sizeof(LongMessage));
  strcat_P(LongMessage,(PGM_P)F("["));
  for(int i=LogDepth-1;i >=0 ; i--)
  {
   strcat_P(LongMessage,(PGM_P)F("\""));
   strcat(LongMessage,Logs[i]);
   strcat_P(LongMessage,(PGM_P)F("\""));
   if(i > 0 ) strcat_P(LongMessage,(PGM_P)F(","));
  }
  LongMessage[strlen(LongMessage)] = ']';
  return LongMessage;
}

//////////////////////////////////////////////////////////////////
//Settings
void GrowBox::setDebugOnOff(bool State){
  BoxSettings -> DebugEnabled = State;
  if(BoxSettings -> DebugEnabled){ 
    addToLog(F("Debug enabled"));
    Sound1 -> playOnSound();
    }
  else {
    addToLog(F("Debug disabled"));
    Sound1 -> playOffSound();
    }
}

void GrowBox::setMetricSystemEnabled(bool MetricEnabled){ 
  if(MetricEnabled != BoxSettings -> MetricSystemEnabled){  //if there was a change
    BoxSettings -> MetricSystemEnabled = MetricEnabled;
    //BoxSettings -> InFanSwitchTemp = convertBetweenTempUnits(BoxSettings -> InFanSwitchTemp);    
    Pressure1 -> Pressure -> resetAverage();
    InDHT -> Temp -> resetAverage(); 
    ExDHT -> Temp -> resetAverage();
    WaterTemp1 -> Temp -> resetAverage();
    refreshRequest = true; 
  }    
  if(BoxSettings -> MetricSystemEnabled) addToLog(F("Metric units"));
  else addToLog(F("Imperial units"));  
}

//////////////////////////////////////////////////////////////////
//Google Sheets reporting

void GrowBox::setReportToGoogleSheetsOnOff(bool State){
  BoxSettings -> ReportToGoogleSheets = State;
  if(State){ 
    addToLog(F("Google Sheets enabled"));
    Sound1 -> playOnSound();
    }
  else {
    addToLog(F("Google Sheets disabled"));
    Sound1 -> playOffSound();
    }
}

void GrowBox::ReportToGoogleSheets(bool CalledFromWebsite){
  if(BoxSettings -> ReportToGoogleSheets || CalledFromWebsite){
    if(CalledFromWebsite)addToLog(F("Reporting to Sheets"),false);
    memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
    strcat_P(LongMessage,(PGM_P)F("/pushingbox?devid="));  
    strcat(LongMessage,BoxSettings -> PushingBoxLogRelayID);
    strcat_P(LongMessage,(PGM_P)F("&BoxData={\"Log\":{"));   
    strcat_P(LongMessage,(PGM_P)F("\"InternalTemp\":\""));  strcat(LongMessage,InDHT -> getTempText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"ExternalTemp\":\""));  strcat(LongMessage,ExDHT -> getTempText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"InternalHumidity\":\""));  strcat(LongMessage,InDHT -> getHumidityText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"ExternalHumidity\":\""));  strcat(LongMessage,ExDHT -> getHumidityText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"InternalFan\":\"")); strcat(LongMessage,InFan -> fanSpeedToNumber());
    strcat_P(LongMessage,(PGM_P)F("\",\"ExhaustFan\":\"")); strcat(LongMessage,ExFan -> fanSpeedToNumber());
    strcat_P(LongMessage,(PGM_P)F("\",\"Light1_Status\":\""));  strcat(LongMessage,Light1 -> getStatusText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"Light1_Brightness\":\""));  strcat(LongMessage,Light1 -> getBrightness());
    strcat_P(LongMessage,(PGM_P)F("\",\"LightReading\":\""));  strcat(LongMessage,LightSensor1 -> getReadingText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"Dark\":\""));  strcat(LongMessage,LightSensor1 -> getDarkText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"WaterLevel\":\""));  strcat(LongMessage,WaterLevel1 -> getLevelText());
    strcat_P(LongMessage,(PGM_P)F("\",\"WaterTemp\":\""));  strcat(LongMessage,WaterTemp1 -> getTempText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"PH\":\""));  strcat(LongMessage,PHSensor1 -> getPHText());  
    strcat_P(LongMessage,(PGM_P)F("\",\"Pressure\":\""));  strcat(LongMessage,Pressure1 -> getPressureText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"Power\":\""));  strcat(LongMessage,Power1 -> getPowerText(false)); 
    strcat_P(LongMessage,(PGM_P)F("\",\"Energy\":\""));  strcat(LongMessage,Power1 -> getEnergyText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"Voltage\":\""));  strcat(LongMessage,Power1 -> getVoltageText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"Current\":\""));  strcat(LongMessage,Power1 -> getCurrentText(false));
    //strcat_P(LongMessage,(PGM_P)F("\",\"Frequency\":\""));  strcat(LongMessage,Power1 -> getFrequencyText(false));   //Only for PZEM004T V3.0
    //strcat_P(LongMessage,(PGM_P)F("\",\"PowerFactor\":\""));  strcat(LongMessage,Power1 -> getPowerFactorText());    //Only for PZEM004T V3.0
    strcat_P(LongMessage,(PGM_P)F("\",\"Light1_Timer\":\""));  strcat(LongMessage,Light1 -> getTimerOnOffText(false));
    strcat_P(LongMessage,(PGM_P)F("\",\"Light1_OnTime\":\""));  strcat(LongMessage,Light1 -> getOnTimeText());
    strcat_P(LongMessage,(PGM_P)F("\",\"Light1_OffTime\":\""));  strcat(LongMessage,Light1 -> getOffTimeText());
    strcat_P(LongMessage,(PGM_P)F("\",\"AeroInterval\":\"")); strcat(LongMessage,Aero_NT1 -> getInterval());
    strcat_P(LongMessage,(PGM_P)F("\",\"AeroDuration\":\"")); strcat(LongMessage,Aero_NT1 -> getDuration());
    //strcat_P(LongMessage,(PGM_P)F("\",\"AeroInterval\":\"")); strcat(LongMessage,Aero_T1 -> getInterval());
    //strcat_P(LongMessage,(PGM_P)F("\",\"AeroDuration\":\"")); strcat(LongMessage,Aero_T1 -> getDuration());  
    
    strcat_P(LongMessage,(PGM_P)F("\"},\"Settings\":{"));
    strcat_P(LongMessage,(PGM_P)F("\"Metric\":\""));  strcat(LongMessage,toText(BoxSettings -> MetricSystemEnabled));
    strcat_P(LongMessage,(PGM_P)F("\"}}"));
    if(BoxSettings -> DebugEnabled){ //print the report command to console
      logToSerials(F("api.pushingbox.com"),false,4);
      logToSerials(&LongMessage,true,0); 
    }
    PushingBoxRestAPI.get(LongMessage); //PushingBoxRestAPI will append http://api.pushingbox.com/ in front of the command
  }
}

//{parameter={Log={"Report":{"InternalTemp":"20.84","ExternalTemp":"20.87","InternalHumidity":"38.54","ExternalHumidity":"41.87","InternalFan":"0","ExhaustFan":"0","Light1_Status":"0","Light1_Brightness":"15","LightReading":"454","Dark":"1","WaterLevel":"0","WaterTemp":"20.56","PH":"17.73","Pressure":"-0.18","Power":"-1.00","Energy":"-0.00","Voltage":"-1.00","Current":"-1.00","Light1_Timer":"1","Light1_OnTime":"04:20","Light1_OffTime":"16:20","AeroInterval":"15","AeroDuration":"2"},"Settings":{"Metric":"1"}}}, contextPath=, contentLength=499, queryString=, parameters={Log=[Ljava.lang.Object;@60efa46b}, postData=FileUpload}

void GrowBox::setPushingBoxLogRelayID(char * ID){  
  strncpy(BoxSettings -> PushingBoxLogRelayID,ID,MaxTextLength);
  addToLog(F("Sheets log relay ID updated"));
}