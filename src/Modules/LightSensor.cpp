#include "LightSensor.h"
#include "Lights.h"
#include "../../GrowBox.h"

LightSensor::LightSensor(const __FlashStringHelper * Name, GrowBox * GBox, Settings::LightSensorSettings * DefaultSettings, Lights * LightSource): Common(Name){ //constructor
  this -> GBox = GBox;
  this -> LightSource = LightSource;
  this -> DigitalPin = &DefaultSettings -> DigitalPin;
  this -> AnalogPin = &DefaultSettings -> AnalogPin;
  pinMode(*DigitalPin, INPUT);
  pinMode(*AnalogPin, INPUT);
  LightReading = new RollingAverage();
  triggerCalibration();
  GBox -> AddToReportQueue(this);  //Subscribing to the report queue: Calls the report() method
  GBox -> AddToRefreshQueue_Minute(this);  //Subscribing to the Minute refresh queue: Calls the refresh_Minute() method
  GBox -> AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event
  GBox -> AddToWebsiteQueue_Load(this); //Subscribing to the Website refresh event
  GBox -> AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event
  logToSerials(F("LightSensor object created"),true,1);
}

void LightSensor::websiteEvent_Refresh(__attribute__((unused)) char * url){ //When the website is refreshed (5sec)
   if (strcmp(url,"/GrowBox.html.json")==0){
    WebServer.setArgString(getWebsiteComponentName(F("Dark")),getDarkText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Reading")),getReadingText(true,false));
  }
}

void LightSensor::websiteEvent_Load(__attribute__((unused)) char * url){ //When the website is opened, load stuff once 
  if (strcmp(url,"/GrowBox.html.json")==0){
    WebServer.setArgString(getWebsiteComponentName(F("Calibrated")),getCalibrationText());
  }
} 

void LightSensor::websiteEvent_Button(char * Button){  //When a button is pressed on the website
  if(!isThisMyComponent(Button)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name     
    if (strcmp_P(ShortMessage,(PGM_P)F("Calibrate"))==0) {triggerCalibration();}
  }  
} 

void LightSensor::refresh_Minute(){  //Called when component should refresh its state
  if(GBox -> BoxSettings -> DebugEnabled) Common::refresh_Minute();
  if(CalibrateRequested){ calibrate(); } //If calibration was requested
  Dark = digitalRead(*DigitalPin); //digitalRead has negative logic: 0- light detected , 1 - no light detected.
  LightReading -> updateAverage(1023 - analogRead(*AnalogPin));
}

void LightSensor::report(){
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("Dark:")); strcat(LongMessage,getDarkText(true));
  strcat_P(LongMessage,(PGM_P)F(" ; LightReading:")); strcat(LongMessage, getReadingText(true,true));  
  logToSerials( &LongMessage, true,1);
}

void LightSensor::triggerCalibration(){ //website signals to calibrate light sensor MAX and MIN readings the next time a refresh runs
  CalibrateRequested = true; 
}

void LightSensor::calibrate(){
  CalibrateRequested=false;  
  bool LastStatus = LightSource -> getStatus();  //TODO: This should be more generic and support different Lights objects passed as a parameter
  byte LastBrightness = *(LightSource -> Brightness);
  LightSource -> setLightOnOff(false,false);  //turn off light, without adding a log entry
  delay(500); //wait for light output change
  DarkReading = 1023 - analogRead(*AnalogPin);
  LightSource -> setBrightness(0,false);
  LightSource -> setLightOnOff(true,false);  //turn on light, without adding a log entry
  delay(500); //wait for light output change
  MinReading = 1023 - analogRead(*AnalogPin);
  LightSource -> setBrightness(100,false);
  delay(500); //wait for light output change
  MaxReading = 1023 - analogRead(*AnalogPin);
  LightSource -> setBrightness(LastBrightness,false); //restore original brightness, without adding a log entry
  LightSource -> setLightOnOff(LastStatus,false); //restore original state, without adding a log entry
  GBox -> addToLog(F("Lights calibrated"),4);
  if(GBox -> BoxSettings -> DebugEnabled){
         logToSerials(F("OFF - "),false,4); logToSerials(&MinReading,false,0);
         logToSerials(F(", 0% - "),false,0); logToSerials(&MinReading,false,0);
         logToSerials(F(", 100% - "),false,0); logToSerials(&MaxReading,true,0);
  }
}

char * LightSensor::getCalibrationText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,toText(DarkReading));   
  strcat_P(ReturnChar,(PGM_P)F(" / ")); 
  strcat(ReturnChar,toText(MinReading));
  strcat_P(ReturnChar,(PGM_P)F(" / "));
  strcat(ReturnChar,toText(MaxReading));   
  return ReturnChar;  
}

int LightSensor::getReading(bool ReturnAverage){ 
  return LightReading -> getInt(ReturnAverage);
}

char * LightSensor::getReadingText(bool IncludePercentage, bool ReturnAverage){
  if(IncludePercentage){
    static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
    memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
    strcat(ReturnChar,LightReading -> getIntText(ReturnAverage));   
    strcat_P(ReturnChar,(PGM_P)F(" [")); 
    if(Dark) strcat(ReturnChar, percentageToText(map(LightReading -> getInt(ReturnAverage),DarkReading,MinReading,0,100))); //https://www.arduino.cc/reference/en/language/functions/math/map/ 
    else strcat(ReturnChar, percentageToText(map(LightReading -> getInt(ReturnAverage),MinReading,MaxReading,0,100)));
    strcat_P(ReturnChar,(PGM_P)F("]"));   
    return ReturnChar;
  }
  else return LightReading -> getIntText(ReturnAverage);
}

bool LightSensor::getDark(){ //Light sensor digital feedback: True(Dark) or False(Bright)  
  return Dark;
}

char * LightSensor::getDarkText(bool UseWords){
  if(UseWords) return yesNoToText(Dark);
  else return toText(Dark);
}