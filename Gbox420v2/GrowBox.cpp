#include "GrowBox.h"
#include "DHTSensor.h"
#include "Lights.h"
#include "Sound.h"
#include "Fan.h"
#include "PowerSensor.h"
#include "LightSensor.h"
#include "PHSensor.h" 
#include "LightSensor.h"
#include "PressureSensor.h" 
#include "Aeroponics_Tank.h" 
#include "Aeroponics_NoTank.h" 

static char Logs[LogDepth][MaxTextLength];  //two dimensional array for storing log histroy displayed on the website (array of char arrays)

GrowBox::GrowBox(const __FlashStringHelper * Name, Settings *BoxSettings): Common(Name) { //Constructor
  this -> BoxSettings = BoxSettings;
  Sound1 = new Sound(F("Sound1"), this, &BoxSettings -> Sound1);
  InternalFan = new Fan(F("InternalFan"), this, &BoxSettings -> InternalFan);
  ExhaustFan = new Fan(F("ExhaustFan"), this, &BoxSettings -> ExhaustFan);
  Light1 = new Lights(F("Light1"), this, &BoxSettings -> Light1);   //Passing BoxSettings members as references: Changes get written back to BoxSettings and saved to EEPROM. (byte *)(((byte *)&BoxSettings) + offsetof(Settings, LightOnHour))
  LightSensor1 = new LightSensor(F("LightSensor1"), this, &BoxSettings -> LightSensor1);
  PowerSensor1 = new PowerSensor(F("PowerSensor1"), this, &Serial2);
  InternalDHTSensor = new DHTSensor(F("InternalDHTSensor"), this, &BoxSettings -> InternalDHTSensor);  //passing: Component name, GrowBox object the component belongs to, Default settings)
  ExternalDHTSensor = new DHTSensor(F("ExternalDHTSensor"), this, &BoxSettings -> ExternalDHTSensor);  //passing: Component name, GrowBox object the component belongs to, Default settings)
  //Aeroponics_Tank1 = new Aeroponics_Tank(F("Aeroponics_Tank1"), this, &BoxSettings ->Aeroponics_Tank1_Common, &BoxSettings -> Aeroponics_Tank1_Specific);
  Aeroponics_NoTank1 = new Aeroponics_NoTank(F("Aeroponics_NoTank1"), this, &BoxSettings -> Aeroponics_NoTank1_Common, &BoxSettings -> Aeroponics_NoTank1_Specific);
  PressureSensor1 = new PressureSensor(F("PressureSensor1"),this,&BoxSettings -> PressureSensor1);
  //PHSensor1 = new PHSensor(this, BoxSettings -> PHSensorInPin,);  
  AddToRefreshQueue_FiveSec(this);  //Subscribing to the Minute refresh queue: Calls the refresh() method  
  AddToWebsiteQueue_Load(this); //Subscribing to the Website load event
  AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event
  AddToWebsiteQueue_Field(this); //Subscribing to the Website field submit event
  AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event
  logToSerials(F("GrowBox object created"), true,2);
  addToLog(F("GrowBox initialized"),0);
}

void GrowBox::websiteEvent_Refresh(__attribute__((unused)) char * url) //called when website is refreshed.
{ 
  WebServer.setArgJson(F("list_SerialLog"), eventLogToJSON(false)); //Last events that happened in JSON format  
}

void GrowBox::websiteEvent_Load(__attribute__((unused)) char * url){ //When the website is opened, load stuff once
  if (strcmp(url,"/Settings.html.json")==0){  
  WebServer.setArgInt(F("GBox1_DebugEnabled"), GBox -> BoxSettings -> DebugEnabled);
  WebServer.setArgInt(F("GBox1_MetricSystemEnabled"), GBox -> BoxSettings -> MetricSystemEnabled);
  WebServer.setArgBoolean(F("GBox1_MqttEnabled"), GBox -> BoxSettings -> ReportToMqtt);
  WebServer.setArgBoolean(F("GBox1_GoogleSheetsEnabled"), GBox -> BoxSettings -> ReportToGoogleSheets);
  WebServer.setArgString(F("GBox1_PushingBoxLogRelayID"), GBox -> BoxSettings -> PushingBoxLogRelayID);
  WebServer.setArgBoolean(F("GBox1_AlertEmails"), GBox -> BoxSettings -> AlertEmails);
  //WebServer.setArgString(F("PushingBoxEmailRelayID"), GBox -> BoxSettings -> PushingBoxEmailRelayID);
  //WebServer.setArgInt(F("TriggerCountBeforeAlert"), GBox -> BoxSettings -> TriggerCountBeforeAlert);
  //WebServer.setArgInt(F("TempAlertLow"), GBox -> BoxSettings -> TempAlertLow);
  //WebServer.setArgInt(F("TempAlertHigh"), GBox -> BoxSettings -> TempAlertHigh);
  //WebServer.setArgInt(F("HumidityAlertLow"), GBox -> BoxSettings -> HumidityAlertLow);
  //WebServer.setArgInt(F("HumidityAlertHigh"), GBox -> BoxSettings -> HumidityAlertHigh);
  //WebServer.setArgString(F("PressureAlertLow"), toText(GBox -> BoxSettings -> PressureAlertLow));
  //WebServer.setArgString(F("PressureAlertHigh"), toText(GBox -> BoxSettings -> PressureAlertHigh));
  //WebServer.setArgString(F("PHAlertLow"), toText(GBox -> Reservoir -> PHAlertLow));
  //WebServer.setArgString(F("PHAlertHigh"), toText(GBox -> BoxSettings -> PHAlertHigh));
  //WebServer.setArgString(F("PHCalibrationSlope"), toPrecisionText(GBox -> BoxSettings -> PHCalibrationSlope));
  //WebServer.setArgString(F("PHCalibrationIntercept"), toPrecisionText(GBox -> BoxSettings -> PHCalibrationIntercept)); 
  //WebServer.setArgString(F("PressureSensorOffset"), toPrecisionText(GBox -> BoxSettings -> PressureSensorOffset));
  //WebServer.setArgString(F("PressureSensorRatio"), toPrecisionText(GBox -> BoxSettings -> PressureSensorRatio));  
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
    //else if(strcmp_P(ShortMessage,(PGM_P)F("PushingBoxLogRelayID"))==0) {setPushingBoxLogRelayID(WebServer.getArgString());}    
  }  
} 


void GrowBox::websiteEvent_Button(char * Button){ //When a button is pressed on the website
  if(!isThisMyComponent(Button)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name   
    if(strcmp_P(ShortMessage,(PGM_P)F("GoogleSheetsTrigger"))==0) {ReportToGoogleSheets(true);}    
  }  
}  


void GrowBox::refresh(){
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable 
  strcat(LongMessage,getFormattedTime());
  /*  
  strcat_P(LongMessage,(PGM_P)F("\n\r Reservoir - "));
  strcat_P(LongMessage,(PGM_P)F("Temp:")); strcat(LongMessage,toText(ReservoirTemp));  
  strcat_P(LongMessage,(PGM_P)F(" ; PH:")); strcat(LongMessage,toText(PH));
  strcat_P(LongMessage,(PGM_P)F("(")); strcat(LongMessage,toText(PHRaw));strcat_P(LongMessage,(PGM_P)F(")"));
  strcat_P(LongMessage,(PGM_P)F(" ; Reservoir:")); strcat(LongMessage,ReservoirText);
 */
  logToSerials( &LongMessage, true,0);
}

void GrowBox::refreshAll(){  //implementing the virtual refresh function from Common
  logToSerials(F("GrowBox refreshing"),true,0);
  //trigger all threads at startup
  runFiveSec(); //needs to run first to get sensor readings
  runSec();  
  runMinute();
  runHalfHour();
}

void GrowBox::runSec(){ 
  //if(DebugEnabled)logToSerials(F("One sec trigger.."),true,1);  
  for(int i=0;i<refreshQueueLength_Sec;i++){
   RefreshQueue_Sec[i] -> refresh();
  }
}

void GrowBox::runFiveSec(){
  if(DebugEnabled)logToSerials(F("Five sec trigger.."),true,1); 
  for(int i=0;i<refreshQueueLength_FiveSec;i++){
    RefreshQueue_FiveSec[i] -> refresh();
  }
}

void GrowBox::runMinute(){
  if(DebugEnabled)logToSerials(F("Minute trigger.."),true,1);
  for(int i=0;i<refreshQueueLength_Minute;i++){
    RefreshQueue_Minute[i] -> refresh();
  }
}

void GrowBox::runHalfHour(){   
  if(DebugEnabled)logToSerials(F("Half hour trigger.."),true,1);
  for(int i=0;i<refreshQueueLength_HalfHour;i++){
    RefreshQueue_HalfHour[i] -> refresh();
  } 
}

//Refresh queues

void GrowBox::AddToRefreshQueue_Sec(Common* Component){
  if(QueueDepth>refreshQueueLength_Sec) RefreshQueue_Sec[refreshQueueLength_Sec++] = Component;
  else addToLog(F("RefreshQueue_Sec overflow!"));  //Too many components are added to the queue, increase "QueueDepth" variable in 420Settings.h , or shift components to a different queue 
}

void GrowBox::AddToRefreshQueue_FiveSec(Common* Component){
  if(QueueDepth>refreshQueueLength_FiveSec) RefreshQueue_FiveSec[refreshQueueLength_FiveSec++] = Component;
  else addToLog(F("RefreshQueue_FiveSec overflow!"));  //Too many components are added to the queue, increase "QueueDepth" variable in 420Settings.h , or shift components to a different queue 
}

void GrowBox::AddToRefreshQueue_Minute(Common* Component){
  if(QueueDepth>refreshQueueLength_Minute) RefreshQueue_Minute[refreshQueueLength_Minute++] = Component;
  else addToLog(F("RefreshQueue_Minute overflow!"));  //Too many components are added to the queue, increase "QueueDepth" variable in 420Settings.h , or shift components to a different queue 
}

void GrowBox::AddToRefreshQueue_HalfHour(Common* Component){
  if(QueueDepth>refreshQueueLength_HalfHour) RefreshQueue_HalfHour[refreshQueueLength_HalfHour++] = Component;
  else addToLog(F("RefreshQueue_HalfHour overflow!"));  //Too many components are added to the queue, increase "QueueDepth" variable in 420Settings.h , or shift components to a different queue 
}

//Website queues
void GrowBox::AddToWebsiteQueue_Load(Common* Component){
  if(QueueDepth>WebsiteQueueLength_Load) WebsiteQueue_Load[WebsiteQueueLength_Load++] = Component;
  else addToLog(F("WebsiteQueueLength_Load overflow!"));  //Too many components are added to the queue, increase "QueueDepth" variable in 420Settings.h , or shift components to a different queue 
}

void GrowBox::AddToWebsiteQueue_Refresh(Common* Component){
  if(QueueDepth>WebsiteQueueLength_Refresh) WebsiteQueue_Refresh[WebsiteQueueLength_Refresh++] = Component;
  else addToLog(F("WebsiteQueueLength_Refresh overflow!"));  //Too many components are added to the queue, increase "QueueDepth" variable in 420Settings.h , or shift components to a different queue 
}

void GrowBox::AddToWebsiteQueue_Button(Common* Component){
  if(QueueDepth>WebsiteQueueLength_Button) WebsiteQueue_Button[WebsiteQueueLength_Button++] = Component;
  else addToLog(F("WebsiteQueueLength_Button overflow!"));  //Too many components are added to the queue, increase "QueueDepth" variable in 420Settings.h , or shift components to a different queue 
}

void GrowBox::AddToWebsiteQueue_Field(Common* Component){
  if(QueueDepth>WebsiteQueueLength_Field) WebsiteQueue_Field[WebsiteQueueLength_Field++] = Component;
  else addToLog(F("WebsiteQueueLength_Field overflow!"));  //Too many components are added to the queue, increase "QueueDepth" variable in 420Settings.h , or shift components to a different queue 
}

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

char* GrowBox::eventLogToJSON(bool Append){ //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
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

//Settings
void GrowBox::setDebugOnOff(bool State){
  BoxSettings -> DebugEnabled = State;
  if(BoxSettings -> DebugEnabled){ 
    addToLog(F("Debug LongMessages enabled"));
    Sound1 -> playOnSound();
    }
  else {
    addToLog(F("Debug LongMessages disabled"));
    Sound1 -> playOffSound();
    }
}

void GrowBox::setMetricSystemEnabled(bool MetricEnabled){ 
  if(MetricEnabled != BoxSettings -> MetricSystemEnabled){  //if there was a change
    BoxSettings -> MetricSystemEnabled = MetricEnabled;
    //BoxSettings -> InternalFanSwitchTemp = convertBetweenTempUnits(BoxSettings -> InternalFanSwitchTemp);
    BoxSettings -> InternalDHTSensor.TempAlertLow= convertBetweenTempUnits(BoxSettings -> InternalDHTSensor.TempAlertLow);
    BoxSettings -> InternalDHTSensor.TempAlertHigh= convertBetweenTempUnits(BoxSettings -> InternalDHTSensor.TempAlertHigh);
    BoxSettings -> ExternalDHTSensor.TempAlertLow= convertBetweenTempUnits(BoxSettings -> ExternalDHTSensor.TempAlertLow);
    BoxSettings -> ExternalDHTSensor.TempAlertHigh= convertBetweenTempUnits(BoxSettings -> ExternalDHTSensor.TempAlertHigh);
    BoxSettings -> PressureSensor1.AlertLow=convertBetweenPressureUnits(BoxSettings -> PressureSensor1.AlertLow);
    BoxSettings -> PressureSensor1.AlertHigh=convertBetweenPressureUnits(BoxSettings -> PressureSensor1.AlertHigh);
    PressureSensor1 -> Pressure -> resetAverage();
    InternalDHTSensor -> Temp -> resetAverage(); 
    ExternalDHTSensor -> Temp -> resetAverage();
    refreshAll(); 
  }    
  if(BoxSettings -> MetricSystemEnabled) addToLog(F("Using Metric units"));
  else addToLog(F("Using Imperial units"));  
}

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

void GrowBox::ReportToGoogleSheets(bool AddToLog){
  if(AddToLog)addToLog(F("Reporting to Google Sheets"));
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("/pushingbox?devid=")); strcat(LongMessage,BoxSettings -> PushingBoxLogRelayID);
  strcat_P(LongMessage,(PGM_P)F("&Log="));logToJSON(false,true);  
  logToSerials(F("Reporting to Google Sheets: "),false); logToSerials(LongMessage,true);   
  RestAPI.get(LongMessage);
}

char* GrowBox::logToJSON(bool AddToLog,bool Append){ //publish readings in JSON format
  if(AddToLog)addToLog(F("Reporting to Google Sheets"));
  if(!Append)memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("{\"BoxDate\":\""));  strcat(LongMessage,getFormattedTime());
  strcat_P(LongMessage,(PGM_P)F("\",\"InternalTemp\":\""));  strcat(LongMessage,InternalDHTSensor -> getTempText() );
  strcat_P(LongMessage,(PGM_P)F("\",\"ExternalTemp\":\""));  strcat(LongMessage,ExternalDHTSensor -> getTempText() );
  strcat_P(LongMessage,(PGM_P)F("\",\"InternalHumidity\":\""));  strcat(LongMessage,InternalDHTSensor -> getHumidityText() );
  strcat_P(LongMessage,(PGM_P)F("\",\"ExternalHumidity\":\""));  strcat(LongMessage,InternalDHTSensor -> getHumidityText() );
  strcat_P(LongMessage,(PGM_P)F("\",\"InternalFan\":\"")); strcat_P(LongMessage,(PGM_P)InternalFan -> fanSpeedToNumber());
  strcat_P(LongMessage,(PGM_P)F("\",\"ExhaustFan\":\"")); strcat_P(LongMessage,(PGM_P)ExhaustFan -> fanSpeedToNumber());
  strcat_P(LongMessage,(PGM_P)F("\",\"Power\":\""));  strcat(LongMessage,PowerSensor1 -> getPowerText()); 
  strcat_P(LongMessage,(PGM_P)F("\",\"Energy\":\""));  strcat(LongMessage,PowerSensor1 -> getEnergyText());
  strcat_P(LongMessage,(PGM_P)F("\",\"Voltage\":\""));  strcat(LongMessage,PowerSensor1 -> getVoltageText());
  strcat_P(LongMessage,(PGM_P)F("\",\"Current\":\""));  strcat(LongMessage,PowerSensor1 -> getCurrentText());
  strcat_P(LongMessage,(PGM_P)F("\",\"Frequency\":\""));  strcat(LongMessage,PowerSensor1 -> getFrequencyText());
  strcat_P(LongMessage,(PGM_P)F("\",\"PowerFactor\":\""));  strcat(LongMessage,PowerSensor1 -> getPowerFactorText());
  strcat_P(LongMessage,(PGM_P)F("\",\"Light1\":\""));  strcat(LongMessage,Light1 -> getStatusText());
  strcat_P(LongMessage,(PGM_P)F("\",\"Light1_Status\":\""));  strcat(LongMessage,Light1 -> getStatusText());
  strcat_P(LongMessage,(PGM_P)F("\",\"Light1_Brightness\":\""));  strcat(LongMessage,Light1 -> getBrightness());
  strcat_P(LongMessage,(PGM_P)F("\",\"Light1_Timer\":\""));  strcat(LongMessage,Light1 -> getTimerOnOffText());
  strcat_P(LongMessage,(PGM_P)F("\",\"Light1_OnTime\":\""));  strcat(LongMessage,Light1 -> getOnTimeText());
  strcat_P(LongMessage,(PGM_P)F("\",\"Light1_OffTime\":\""));  strcat(LongMessage,Light1 -> getOffTimeText());
  strcat_P(LongMessage,(PGM_P)F("\",\"LightReading\":\""));  strcat(LongMessage,LightSensor1 -> getReadingText());
  strcat_P(LongMessage,(PGM_P)F("\",\"IsDark\":\""));  strcat(LongMessage,LightSensor1 -> getIsDarkText());
  //strcat_P(LongMessage,(PGM_P)F("\",\"Reservoir\":\""));  strcat(LongMessage,toText(ReservoirLevel));
  //strcat_P(LongMessage,(PGM_P)F("\",\"PH\":\""));  strcat(LongMessage,toText(PH));
  //strcat_P(LongMessage,(PGM_P)F("\",\"ReservoirTemp\":\""));  strcat(LongMessage,toText(ReservoirTemp));
  strcat_P(LongMessage,(PGM_P)F("\",\"Pressure\":\""));  strcat(LongMessage,PressureSensor1 -> getPressureText());
  strcat_P(LongMessage,(PGM_P)F("\",\"AeroInterval\":\"")); strcat(LongMessage,Aeroponics_NoTank1 -> getInterval());
  strcat_P(LongMessage,(PGM_P)F("\",\"AeroDuration\":\"")); strcat(LongMessage,Aeroponics_NoTank1 -> getDuration());
  //strcat_P(LongMessage,(PGM_P)F("\",\"AeroInterval\":\"")); strcat(LongMessage,Aeroponics_Tank1 -> getInterval());
  //strcat_P(LongMessage,(PGM_P)F("\",\"AeroDuration\":\"")); strcat(LongMessage,Aeroponics_Tank1 -> getDuration());
  strcat_P(LongMessage,(PGM_P)F("\"}"));
  return LongMessage;
}