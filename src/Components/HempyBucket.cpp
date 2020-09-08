#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WaterPump *BucketPump) : Common(Name)
{
  this->Parent = Parent;
  this->BucketWeightSensor = BucketWeightSensor;
  this->BucketPump = BucketPump;
  WeightBasedWatering = &DefaultSettings->WeightBasedWatering;
  StartWeight = &DefaultSettings->StartWeight;
  StopWeight = &DefaultSettings->StopWeight;
  TimerBasedWatering = &DefaultSettings->TimerBasedWatering;
  WateringInterval = &DefaultSettings->WateringInterval;
  WateringDuration = &DefaultSettings->WateringDuration;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("Hempy bucket object created"), true, 1);
}

void HempyBucket::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  checkWateringWeight();
  checkWateringTimer();
}

void HempyBucket::refresh_Sec()
{
  if (BucketPump->getOnState())
    checkWateringFinished(); ///When pump is on check if it is time to stop
}

void HempyBucket::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Weight based watering:"));
  strcat(LongMessage, toText_enabledDisabled(*WeightBasedWatering));
  strcat_P(LongMessage, (PGM_P)F(" ; Start weight:"));
  strcat(LongMessage, toText_weight(*StartWeight));
  strcat_P(LongMessage, (PGM_P)F(" ; Stop weight:"));
  strcat(LongMessage, toText_weight(*StopWeight));
  strcat_P(LongMessage, (PGM_P)F(" ; Timer based watering:"));
  strcat(LongMessage, toText_enabledDisabled(*TimerBasedWatering));
  strcat_P(LongMessage, (PGM_P)F(" ; Interval:"));
  strcat(LongMessage, toText_minute(*WateringInterval));
  strcat_P(LongMessage, (PGM_P)F(" ; Duration:"));
  strcat(LongMessage, toText_second(*WateringDuration));
  logToSerials(&LongMessage, true, 1);
}

void HempyBucket::checkWateringWeight()
{
  BucketWeightSensor->readWeight();   //Force a weight refresh
  if (*WeightBasedWatering && BucketWeightSensor->getWeight() < *StartWeight && !BucketPump->getOnState() && BucketPump->getEnabledState()) ///If the weight based watering is enabled AND the weight is below the limit AND the pump is off AND pump is enabled
  {
    logToSerials(F("Weight based watering..."), true, 1);
    BucketPump->startPump();
    WateringTimer = millis();
    WateringTrigger = WateringMode::WEIGHT;
  }
}

void HempyBucket::checkWateringTimer()
{
  if (*TimerBasedWatering && millis() - WateringTimer >= ((uint32_t)*WateringInterval * 60000))
  {
    logToSerials(F("Timer based watering..."), true, 1);
    BucketPump->startPump();
    WateringTimer = millis();
    WateringTrigger = WateringMode::TIMER;
  }
}

void HempyBucket::checkWateringFinished()
{
  bool WeightReached = false; // Signals when the weight target is reached
  bool TimerReached = false; // Signals when the watering timer is finished
  if(WateringTrigger == WateringMode::MANUAL)  //If watering was triggered by user
  {
    if(*WeightBasedWatering) //Weight based watering enabled
    {
      BucketWeightSensor->readWeight(); 
      if (BucketWeightSensor->getWeight() > *StopWeight) ///If the weight is over the limit and the pump is on
      {
        WeightReached = true;        
      }
    }
    else{  //Weight based watering not enabled 
      WeightReached = true;  //Fake ready signal when weight based watering is turned off 
    } 

    if(*TimerBasedWatering){ //Timer based watering enabled 
      if ( millis() - WateringTimer >= ((uint32_t)*WateringDuration * 1000)) ///If the pump runtime reached the watering duration
      {
        TimerReached = true;
      }
    }
    else{  //Timer based watering not enabled 
      TimerReached = true;  //Fake ready signal when timer based watering is turned off 
    } 
  }
  else if (WateringTrigger == WateringMode::WEIGHT) //< If watering triggered by weight: Run until weight tareget is reached
  {
    TimerReached = true; //Fake ready signal, only weight is considered in this watering mode
    BucketWeightSensor->readWeight();                  //Force a weight refresh
    if (BucketWeightSensor->getWeight() > *StopWeight) ///If the weight is over the limit and the pump is on
    {
      BucketPump->stopPump();
    }
  }
  else //if (WateringTrigger == WateringMode::TIMER)  //< If watering triggered by timer: Run until duration is reached
  {
    WeightReached = true;  //Fake ready signal, only timer is considered in this watering mode
    if (millis() - WateringTimer >= ((uint32_t)*WateringDuration * 1000))
    {
      TimerReached = true; 
    }
  }

  if(WeightReached && TimerReached){
    BucketPump->stopPump();  //< Turn the pump off when all criteria are met
  }
}

void HempyBucket::startWatering()
{
  logToSerials(F("Manual watering..."), true, 1);
  WateringTrigger = WateringMode::MANUAL;
  WateringTimer = millis();
  BucketPump -> startPump(true);
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

void HempyBucket::setWeightBasedWatering(bool State)
{
  if (*WeightBasedWatering != State)
  {
    *WeightBasedWatering = State;
    logToSerials(Name, false, 1);
    logToSerials(F("Weight based watering"), false, 1);
    logToSerials(toText_enabledDisabled(State), true, 1);
    if(*WeightBasedWatering)
    {
      Parent->getSoundObject()->playOnSound();
    }
    else
    {
      Parent->getSoundObject()->playOffSound();
    }    
  }
}

void HempyBucket::setStartWeight(float Weight)
{
  if (*StartWeight != Weight)
  {
    *StartWeight = Weight;
    Parent->getSoundObject()->playOnSound();
  }
}

void HempyBucket::setStopWeight(float Weight)
{
  if (*StopWeight != Weight)
  {
    *StopWeight = Weight;
    logToSerials(Name, false, 1);
    logToSerials(F("Watering limits updated"), true, 1);
    Parent->getSoundObject()->playOnSound();
  }
}

void HempyBucket::setTimerBasedWatering(bool State)
{
  if (*TimerBasedWatering != State)
  {
    *TimerBasedWatering = State;
    logToSerials(Name, false, 1);
    logToSerials(F("Timer based watering"), false, 1);
    logToSerials(toText_enabledDisabled(State), true, 1);
    if(*TimerBasedWatering)
    {
      Parent->getSoundObject()->playOnSound();
    }
    else
    {
      Parent->getSoundObject()->playOffSound();
    }
  }
}

void HempyBucket::setWateringInterval(uint16_t Minutes)
{
  if (*WateringInterval != Minutes)
  {
    *WateringInterval = Minutes;
    Parent->getSoundObject()->playOnSound();
    logToSerials(Name, false, 1);
    logToSerials(F("Watering interval updated"), true, 1);
  }
}

void HempyBucket::setWateringDuration(uint16_t Seconds)
{
  if (*WateringDuration != Seconds)
  {
    *WateringDuration = Seconds;
    Parent->getSoundObject()->playOnSound();
    logToSerials(Name, false, 1);
    logToSerials(F("Watering duration updated"), true, 1);
  }
}