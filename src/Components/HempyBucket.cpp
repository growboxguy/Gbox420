#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings,WeightSensor *FeedbackWeightSensor) : Common(Name)
{
  this->Parent = Parent;
  this->FeedbackWeightSensor = FeedbackWeightSensor;
  PumpPin = &DefaultSettings->PumpPin;
  StartWeight = &DefaultSettings->StartWeight;
  StopWeight = &DefaultSettings->StopWeight;
  Parent->addToReportQueue(this);         ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Minute(this); ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  Parent->addToRefreshQueue_Sec(this);  
  logToSerials(F("Hempy bucket object created"), true, 1);
}

void HempyBucket::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  checkBucketWeight();
}

void HempyBucket::refresh_Sec()
{
  refresh_Minute(); ///temporary for faster testing
}

void HempyBucket::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Pump:"));
  strcat(LongMessage, onOffToText(*PumpOn));
  strcat_P(LongMessage, (PGM_P)F(" ; Reservoir:"));
  strcat(LongMessage, F("100%")); ///Fake value \todo Integrate ultrasonic depth sensor 
  logToSerials(&LongMessage, true, 1);
}

void HempyBucket::readWeight(){
  Weight -> updateAverage(Sensor -> get_units());
}

float HempyBucket::waterNow(bool UserRequested)
{
  if (UserRequest)
  { ///if the pump was turned on from the web interface, not by the automation
    Parent->addToLog(F("Watering..."));
    Parent->getSoundObject()->playOnSound();
  }
  if(FeedbackWeightSensor -> getWeight() < *StopWeight) {
    PumpTimer = millis();
    PumpOn = true;
    checkRelays();
  }
}

void HempyBucket::pumpOff(bool UserRequested)
{
  if (UserRequest)
  { ///if the pump was turned on from the web interface, not by the automation
    Parent->addToLog(F("Pump off"));
    Parent->getSoundObject()->playOffSound();
  }
  PumpOn = false;
  checkRelay();
}

void HempyBucket::checkRelay()
{
  if (PumpOn)
    digitalWrite(*PumpPin, LOW);
  else
    digitalWrite(*PumpPin, HIGH);  
}

char *HempyBucket::getStartWeightText(bool IncludeUnits)
{
  if (IncludeUnits)
    return weightToText(*StartWeight);
  else
    return toText(*StartWeight);
}

char *HempyBucket::getStopWeightText(bool IncludeUnits)
{
  if (IncludeUnits)
    return weightToText(*StopWeight);
  else
    return toText(*StopWeight);
}

void HempyBucket::setStartWeight(float Weight)
{
  *StartWeight = Weight;
}

void HempyBucket::setStopWeight(float Weight)
{
  *StopWeight = Weight;
}