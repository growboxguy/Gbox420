#include "PHSensor.h"
#include "GrowBox.h"

PHSensor::PHSensor (const __FlashStringHelper * Name, GrowBox * GBox,  Settings::PHSensorSettings * DefaultSettings): Common(Name){ //constructor
  this -> GBox = GBox;
  this -> Pin = &DefaultSettings -> Pin;
  this -> Intercept = &DefaultSettings -> Intercept;
  this -> Slope = &DefaultSettings -> Slope;
  pinMode(*Pin, INPUT);
  PH = new RollingAverage();
  GBox -> AddToReportQueue(this);  //Subscribing to the report queue: Calls the report() method
  GBox -> AddToRefreshQueue_Minute(this);  //Subscribing to the FiveSec refresh queue: Calls the refresh() method
  GBox -> AddToWebsiteQueue_Load(this); //Subscribing to the Website load event
  GBox -> AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event
  GBox -> AddToWebsiteQueue_Field(this); //Subscribing to the Website field submit event
  GBox -> AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event
  logToSerials(F("PHSensor object created"),true,1);
}

void PHSensor::websiteEvent_Load(__attribute__((unused)) char * url){ //When the website is opened, load stuff once
  if (strcmp(url,"/Settings.html.json")==0){
    //WebServer.setArgString(F("PHAlertLow"), toText(GBox -> Reservoir -> PHAlertLow));
    //WebServer.setArgString(F("PHAlertHigh"), toText(GBox -> BoxSettings -> PHAlertHigh));
    WebServer.setArgString(getWebsiteComponentName(F("Slope")), toPrecisionText(*Slope));
    WebServer.setArgString(getWebsiteComponentName(F("Intercept")), toPrecisionText(*Intercept));
  } 
}

void PHSensor::websiteEvent_Refresh(__attribute__((unused)) char * url){
  if (strcmp(url,"/GrowBox.html.json")==0){
    WebServer.setArgString(getWebsiteComponentName(F("PH")), getPHText());
  }
}

void PHSensor::websiteEvent_Button(char * Button){ //When a button is pressed on the website
  if(!isThisMyComponent(Button)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name   
    if(strcmp_P(ShortMessage,(PGM_P)F("ReadRaw"))==0) {updatePH(true);}
  }  
} 

void PHSensor::websiteEvent_Field(char * Field){ //When the website is opened, load stuff once
  if(!isThisMyComponent(Field)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name     
    if(strcmp_P(ShortMessage,(PGM_P)F("Slope"))==0) {setSlope(WebServer.getArgFloat());}
    else if(strcmp_P(ShortMessage,(PGM_P)F("Intercept"))==0) {setIntercept(WebServer.getArgFloat());}    
  }  
} 
    
void PHSensor::report(){
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("PH:")); strcat(LongMessage,PH -> getAverageFloatText());  
  logToSerials( &LongMessage, true,1);
}

void PHSensor::refresh_Minute(){
  if(GBox -> BoxSettings -> DebugEnabled) Common::refresh_Minute();
  updatePH(false); 
}

void PHSensor::updatePH(bool ShowRaw){
  int PHRaw = analogRead(*Pin);
  if(ShowRaw)
  {
    strncpy_P(LongMessage,(PGM_P)F("PH analog read: "),MaxTextLength);
    strcat(LongMessage,toText(PHRaw));
    GBox -> addToLog(LongMessage);
  } 
  PH -> updateAverage((*Slope)*PHRaw + (*Intercept)); 
  //checkPHAlert();
}

float PHSensor::getPH(){
  return PH -> getAverageFloat();
}

char * PHSensor::getPHText(){
  return PH -> getAverageFloatText();
}

void PHSensor::setSlope(float Value){
  *Slope = Value;
  GBox -> addToLog(F("PH slope updated"));
}

void PHSensor::setIntercept(float Value){
  *Intercept = Value;
  GBox -> addToLog(F("PH intercept updated"));
}