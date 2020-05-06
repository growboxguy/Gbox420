#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings,WeightSensor *BucketWeightSensor, WaterPump *BucketPump) : Common(Name)
{
  this->Parent = Parent;
  this->BucketWeightSensor = BucketWeightSensor;
  this->BucketPump = BucketPump;
  StartWeight = &DefaultSettings->StartWeight;
  StopWeight = &DefaultSettings->StopWeight;
  Parent->addToReportQueue(this);         ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Sec(this);  
  Parent->addToRefreshQueue_Minute(this); ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
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
 if(BucketPump -> getOnState()) checkBucketWeight(); ///When watering check if its time to stop
}

void HempyBucket::checkBucketWeight()
{  
  BucketWeightSensor -> readWeight();  //Force a weight refresh   
  if(BucketWeightSensor -> getWeight() > *StopWeight && BucketPump -> getOnState()) ///If the weight is over the limit and the pump is on
  {
    BucketPump -> stopPump();     
  }
  else if(BucketWeightSensor -> getWeight() < *StartWeight && !BucketPump -> getOnState())  ///If the weight is below the limit and the pump is off
  {
    BucketPump -> startPump(); 
  }   
}

void HempyBucket::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Start weight:"));
  strcat(LongMessage, toText(*StartWeight));
  strcat_P(LongMessage, (PGM_P)F(" ; Stop weight:"));
  strcat(LongMessage, toText(*StopWeight));  
  logToSerials(&LongMessage, true, 1);
}

float HempyBucket::getStartWeight()
{  
    return *StartWeight;
}

float HempyBucket::getStopWeight()
{
    return *StopWeight;
}


char *HempyBucket::getStartWeightText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_weight(*StartWeight);
  else
    return toText(*StartWeight);
}

char *HempyBucket::getStopWeightText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_weight(*StopWeight);
  else
    return toText(*StopWeight);
}

void HempyBucket::setStartWeight(float Weight)
{
  if(*StartWeight != Weight)
  {
    *StartWeight = Weight;
    Parent -> getSoundObject() -> playOnSound();
  }
}

void HempyBucket::setStopWeight(float Weight)
{
  if(*StopWeight != Weight)
  {
    *StopWeight = Weight;
    logToSerials(Name, false, 1);
    logToSerials(F("Watering limits updated"), true, 1);
    Parent -> getSoundObject() -> playOnSound();
  }
}