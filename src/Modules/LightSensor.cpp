#include "LightSensor.h"
#include "Lights.h"
#include "Sound.h"
#include "../../GrowBox.h"

LightSensor::LightSensor(const __FlashStringHelper *Name, GrowBox *GBox, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource) : Common(Name)
{ //constructor
  this->GBox = GBox;
  this->LightSource = LightSource;
  this->DigitalPin = &DefaultSettings->DigitalPin;
  this->AnalogPin = &DefaultSettings->AnalogPin;
  pinMode(*DigitalPin, INPUT);
  pinMode(*AnalogPin, INPUT);
  LightReading = new RollingAverage();
  calibrate(false);
  GBox->AddToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  GBox->AddToRefreshQueue_Minute(this);  //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  GBox->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  GBox->AddToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  logToSerials(F("LightSensor object created"), true, 1);
}

void LightSensor::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Dark")), getDarkText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Reading")), getReadingText(false));
  }
}

void LightSensor::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Calibrate")) == 0)
    {
      triggerCalibration();
    }
  }
}

void LightSensor::refresh_Minute()
{
  if (GBox->BoxSettings->DebugEnabled)
    Common::refresh_Minute();
  if (CalibrateRequested)
  {
    calibrate();
  }                                //If calibration was requested
  Dark = digitalRead(*DigitalPin); //True: No light detected
  LightReading->updateAverage(1023 - analogRead(*AnalogPin));
}

void LightSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("Dark:"));
  strcat(LongMessage, getDarkText(true));
  strcat_P(LongMessage, (PGM_P)F(" ; LightReading:"));
  strcat(LongMessage, getReadingText(true));
  logToSerials(&LongMessage, true, 1);
}

void LightSensor::triggerCalibration()
{ //website signals to calibrate light sensor MAX and MIN readings the next time a refresh runs
  CalibrateRequested = true;
  GBox->addToLog(F("Calibrating light"));
  GBox->Sound1->playOnSound();
}

void LightSensor::calibrate(bool AddToLog)
{ //Takes ~2sec total, could trigger a watchdog reset!  
  CalibrateRequested = false;
  bool LastStatus = LightSource->getStatus(); //TODO: This should be more generic and support different Lights objects passed as a parameter
  byte LastBrightness = LightSource->getBrightness();
  LightSource->setLightOnOff(false, false); //turn off light, without adding a log entry
  delay(DelaySec);                               //wait for light output change
  Readings[0] = 1023 - analogRead(*AnalogPin);  //store the reading in darkness to the first element of the Readings[10] array
  LightSource->setLightOnOff(true, false); //turn on light, without adding a log entry
  for(byte ReadingCounter=0;ReadingCounter<(ReadingArrayDepth-1);){  //This probably looks dodgy as the 3rd parameter of the for cycle is empty. ReadingCounter is incremented in the code
    LightSource->setBrightness(ReadingCounter++ * 10, false);  //Increment ReadingCounter AFTER reading its value
    wdt_reset();  //Reset watchdog timer before waiting
    delay(DelaySec); //wait for light output change
    Readings[ReadingCounter] = 1023 - analogRead(*AnalogPin); 
  }
  LightSource->setBrightness(LastBrightness, false); //restore original brightness, without adding a log entry
  LightSource->setLightOnOff(LastStatus, false);     //restore original state, without adding a log entry
  getCalibrationReadings(); 
  GBox->relayToGoogleSheets(Name,&LongMessage);
  if (AddToLog)
  {
    GBox->addToLog(F("Lights calibrated"), 4);
  }
}


void LightSensor::getCalibrationReadings(){//Returns a pointer to a char array
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("{\"Readings\":["));
   for(byte ReadingCounter=0;ReadingCounter<ReadingArrayDepth;ReadingCounter++){
     strcat(LongMessage, toText(Readings[ReadingCounter]));
     strcat_P(LongMessage, (PGM_P)F(","));
   }
  LongMessage[strlen(LongMessage)-1] = ']'; //replace the last , to the closing of the array
  strcat_P(LongMessage, (PGM_P)F("}")); //close JSON object
}

int LightSensor::getReading(bool ReturnAverage)  //Gets the average light sensor reading, if passed false as a parameter it returns the latest reading, not the average
{
  return LightReading->getInt(ReturnAverage);
}

char *LightSensor::getReadingText(bool ReturnAverage)
{ 
    return LightReading->getIntText(ReturnAverage);
}

bool LightSensor::getDark()
{
  return Dark;
}

char *LightSensor::getDarkText(bool UseWords)
{
  if (UseWords)
    return yesNoToText(Dark);
  else
    return toText(Dark);
}