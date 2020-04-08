#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings,WeightSensor *FeedbackWeightSensor) : Common(Name)
{
  this->Parent = Parent;
  this->FeedbackWeightSensor = FeedbackWeightSensor;
  PumpPin = &DefaultSettings->PumpPin;
  PumpEnabled = &DefaultSettings->PumpEnabled;
  PumpTimeout= &DefaultSettings->PumpTimeout;
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
  if(PumpOn) checkBucketWeight(); ///When watering check if its time to stop
}

void HempyBucket::checkBucketWeight()
{  
  if(PumpOn)
  {    
    if (millis() - PumpTimer > ((uint32_t)*PumpTimeout * 1000))
    {
      pumpOff(false);
    }
    else
    {  
      FeedbackWeightSensor -> readWeight();  //Force a weight refresh   
      if(FeedbackWeightSensor -> getWeight(false) > *StopWeight)
      {
        pumpOff(false);
      }
    }
  }
  else  ///If pump is off
  {
     if(FeedbackWeightSensor -> getWeight(false) < *StartWeight)
      {
        waterNow(false);
      }
  }
  
}

void HempyBucket::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Pump:"));
  strcat(LongMessage, onOffToText(PumpOn));
  strcat_P(LongMessage, (PGM_P)F(" ; Reservoir:"));
  strcat_P(LongMessage, (PGM_P)F("100%")); ///Fake value \todo Integrate ultrasonic depth sensor 
  logToSerials(&LongMessage, true, 1);
}

void HempyBucket::waterNow(bool UserRequest)
{
  if (UserRequest)
  { ///Provide feedback if the pump was turned on by the user, not by the automation
    Parent->addToLog(F("Watering..."));
    Parent->getSoundObject()->playOnSound();
  }
  if(FeedbackWeightSensor -> getWeight() < *StopWeight) {
    PumpTimer = millis();
    PumpOn = true;
    checkRelay();
  }
}

void HempyBucket::pumpOff(bool UserRequest)
{
  if (UserRequest)
  { ///Provide feedback if the pump was turned off by the user, not by the automation
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