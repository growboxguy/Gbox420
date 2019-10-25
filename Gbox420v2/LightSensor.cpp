#include "LightSensor.h"
#include "Lights.h"
#include "GrowBox.h"

LightSensor::LightSensor(const __FlashStringHelper * Name, GrowBox * GBox,  Settings::LightSensorSettings * DefaultSettings): Common(Name){ //constructor
  this -> GBox = GBox;
  this -> DigitalPin = &DefaultSettings -> DigitalPin;
  this -> AnalogPin = &DefaultSettings -> AnalogPin;
  pinMode(*DigitalPin, INPUT);
  pinMode(*AnalogPin, INPUT);
  LightReading = new RollingAverage();
  triggerCalibration();
  GBox -> AddToRefreshQueue_Minute(this);  //Subscribing to the Minute refresh queue: Calls the refresh() method
  GBox -> AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event
  GBox -> AddToWebsiteQueue_Button(this); //Subscribing to the Website refresh event
  logToSerials(F("LightSensor object created"),true);
}

void LightSensor::websiteRefreshEvent(char * url){ //When the website is opened, load stuff once
  if (strcmp(url,"/GrowBox.html.json")==0){
    WebServer.setArgString(getWebsiteComponentName(F("IsDark")),getIsDarkText());
    WebServer.setArgString(getWebsiteComponentName(F("Reading")),getReadingPercentage());
    WebServer.setArgString(getWebsiteComponentName(F("ReadingRaw")),getReadingText()); 
  }
} 

void LightSensor::websiteButtonPressEvent(char * Button){ //When the website is opened, load stuff once
  if(!isThisMyComponent(Button)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name     
    if (strcmp_P(ShortMessage,(PGM_P)F("Calibrate"))==0) {triggerCalibration();}
  }  
} 

void LightSensor::refresh(){  //Called when component should refresh its state
  Common::refresh();
  if(CalibrateRequested){ calibrate(); } //If calibration was requested
  IsDark = digitalRead(*DigitalPin); //digitalRead has negative logic: 0- light detected , 1 - no light detected. ! inverts this
  LightReading -> updateAverage(1023 - analogRead(*AnalogPin)); 
  report();
}

void LightSensor::report(){
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable
  strcat_P(LongMessage,(PGM_P)F("Light detected:")); strcat(LongMessage,getIsDarkText());
  strcat_P(LongMessage,(PGM_P)F(" ; LightReading:")); strcat(LongMessage, getReadingText());
  strcat_P(LongMessage,(PGM_P)F(" (")); strcat(LongMessage, getReadingPercentage());strcat_P(LongMessage,(PGM_P)F(")"));
  logToSerials( &LongMessage, true,4);
}

//  const char * name_p = reinterpret_cast<const char *>(name);
//   uint8_t nlen = strlen_P(name_p);
//   uint8_t vlen = strlen(value);
//   char buf[nlen+vlen+3];
//   buf[0] = WEB_JSON;
//   strcpy_P(buf+1, name_p);
//   strcpy(buf+2+nlen, value);
//   _elc->Request(buf, nlen+vlen+2); 

char* LightSensor::getReadingText(){
  static char ReturnChar[MaxTextLength] = ""; //each call will overwrite the same variable
  memset(&ReturnChar[0], 0, sizeof(ReturnChar));  //clear variable
  strcat(ReturnChar,LightReading -> getAverageIntText());   
  strcat_P(ReturnChar,(PGM_P)F(" [")); 
  strcat(ReturnChar,toText(MinReading));
  strcat_P(ReturnChar,(PGM_P)F("/"));
  strcat(ReturnChar,toText(MaxReading));
  strcat_P(ReturnChar,(PGM_P)F("]"));   
  return ReturnChar;
}

void LightSensor::triggerCalibration(){ //website signals to calibrate light sensor MAX and MIN readings the next time a refresh runs
  CalibrateRequested = true; 
}

void LightSensor::calibrate(){
  CalibrateRequested=false;  
  bool LastStatus = GBox -> Light1 -> getStatus();  //TODO: This should be more generic and support different Lights objects passed as a parameter
  byte LastBrightness = *(GBox -> Light1 -> Brightness);
  GBox -> Light1 -> setLightOnOff(true,false);  //turn on light, without adding a log entry
  GBox -> Light1 -> checkLightStatus();  //apply turning the lights on
  GBox -> Light1 -> setBrightness(0,false);
  delay(500); //wait for light output change
  MinReading = 1023 - analogRead(*AnalogPin);
  GBox -> Light1 -> setBrightness(100,false);
  delay(500); //wait for light output change
  MaxReading = 1023 - analogRead(*AnalogPin);
  GBox -> Light1 -> setBrightness(LastBrightness,false); //restore original brightness, without adding a log entry
  GBox -> Light1 -> setLightOnOff(LastStatus,false); //restore original state, without adding a log entry
  GBox -> Light1 -> checkLightStatus();
  GBox -> addToLog(F("Lights calibrated"),4);
  if(GBox -> BoxSettings -> DebugEnabled){
         logToSerials(F("0% - "),false,4); logToSerials(&MinReading,false,0);
         logToSerials(F(", 100% - "),false,0); logToSerials(&MaxReading,true,0);
  }
}

bool LightSensor::getIsDark(){ //Light sensor digital feedback: True(Dark) or False(Bright)  
  return IsDark;
}

int LightSensor::getReading(){ 
  return LightReading -> getAverageInt();
}

char * LightSensor::getReadingPercentage(){   
  return percentageToText(map(LightReading -> getAverageInt(),MinReading,MaxReading,0,100)); //https://www.arduino.cc/reference/en/language/functions/math/map/ 
}

char * LightSensor::getIsDarkText(){
  return yesNoToText(IsDark);
}