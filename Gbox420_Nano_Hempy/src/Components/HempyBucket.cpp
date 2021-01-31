#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WeightSensor *WasteReservoirWeightSensor, WaterPump *BucketPump) : Common(Name)
{
  this->Parent = Parent;
  this->BucketWeightSensor = BucketWeightSensor;
  this->BucketPump = BucketPump;
  this->WasteReservoirWeightSensor = WasteReservoirWeightSensor;
  //StartWeight = &DefaultSettings->StartWeight;
  // StopWeight = &DefaultSettings->StopWeight;
  WasteLimit = &DefaultSettings->WasteLimit;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("Hempy bucket object created"), true, 3);
}

void HempyBucket::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  updateState(State);
}

void HempyBucket::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  if (State == HempyStates::WATERING)
  {
    updateState(State);
  }
}

void HempyBucket::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("State:"));
  strcat(LongMessage, toText_hempyState(State));
  //(LongMessage, (PGM_P)F(" ; Stop weight:"));
  //strcat(LongMessage, toText_weight(*StopWeight));
  //strcat_P(LongMessage, (PGM_P)F(" ; Waste limit:"));
  //strcat(LongMessage, toText_weight(*WasteLimit));
  logToSerials(&LongMessage, true, 1);
}

void HempyBucket::updateState(HempyStates NewState)
{
  bool BlockOverWritingState = false; //Used when a state transitions to a new state
  if (State != NewState)
  {
    StateTimer = millis(); ///< Start measuring the time spent in the new State

    memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
    strcat_P(LongMessage, (PGM_P)Name);
    strcat_P(LongMessage, (PGM_P)F(" state: "));
    strcat(LongMessage, toText_hempyState(State));
    strcat_P(LongMessage, (PGM_P)F(" -> "));
    strcat(LongMessage, toText_hempyState(NewState));
    logToSerials(&LongMessage, true, 3);
  }

  BucketWeightSensor->readWeight(false);         ///< Force Bucket weight update
  WasteReservoirWeightSensor->readWeight(false); ///< Force Waste Reservoir weight update

  switch (State)
  {
  case HempyStates::DISABLED:
    if (State != NewState)
    {
      BucketPump->disablePump();
    }
    break;
  case HempyStates::IDLE:
    if (State != NewState)
    {
      BucketPump->stopPump();
    }
    if (NextWateringWeight = 0 || BucketWeightSensor->getWeight() <= NextWateringWeight)
    {
      updateState(HempyStates::WATERING);
      BlockOverWritingState = true;
    }
    break;
  case HempyStates::WATERING:
    if (State != NewState)
    {
      BucketStartWeight = BucketWeightSensor->getWeight(); // Store the bucket weight before starting the pump
      BucketPump->startPump();
      if (State == HempyStates::IDLE) // First time entering the WATERING-DRIAINING cycles
      {
        WateringTimer = millis(); ///Start measuring the total watering time
        WasteReservoirStartWeight = WasteReservoirWeightSensor->getWeight();
      }
    }
    if ((BucketWeightSensor->getWeight(false) - BucketStartWeight) + WasteReservoirWeightSensor->getWeight() - WasteReservoirStartWeight >= OverflowTarget) //Target overflow's worth of water was added, wait for it to drain
    {
      updateState(HempyStates::DRAINING);
      BlockOverWritingState = true;
    }
    if (millis() - WateringTimer > ((uint32_t)WateringTimeout * 1000)) ///< Timeout before the waste target was reached, assume the pump or weight sensor failed
    {
      updateState(HempyStates::DISABLED);
      BlockOverWritingState = true;
    }
    break;
  case HempyStates::DRAINING:
    if (State != NewState)
    {
      BucketPump->stopPump();
    }
    if (millis() - StateTimer > ((uint32_t)DrainWaitTime * 1000)) ///< Waiting for the water to drain
    {
      if (WasteReservoirWeightSensor->getWeight(false) - WasteReservoirStartWeight >= OverflowTarget) //Check if target overflow weight is reached
      {
        NextWateringWeight = BucketWeightSensor->getWeight() - EvaporationTarget;
        updateState(HempyStates::IDLE);
      }
      else
      {
        updateState(HempyStates::WATERING); /// Continue watering
      }
      BlockOverWritingState = true;
    }
    break;
  }

  if (State != NewState && !BlockOverWritingState)
  {
    State = NewState;
  }
}

float HempyBucket::getWasteLimit()
{
  return *WasteLimit;
}

char *HempyBucket::getWasteLimitText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_weight(*WasteLimit);
  else
    return toText(*WasteLimit);
}

void HempyBucket::setWasteLimit(float Weight)
{
  if (*WasteLimit != Weight)
  {
    *WasteLimit = Weight;
    logToSerials(Name, false, 1);
    logToSerials(F("Waste limit updated"), true, 1);
    Parent->getSoundObject()->playOnSound();
  }
}

/*
void HempyBucket::checkWateringWeight()
{
  BucketWeightSensor->readWeight(); ///< Force Bucket weight update 
  WasteReservoirWeightSensor->readWeight(); ///< Force Waste Reservoir weight update  
    
  if (BucketPump->getEnabledState()) ///< If the weight based watering is enabled AND the pump is enabled
  {
    if (BucketWeightSensor->getWeight(true) < *StartWeight && BucketPump->getState() != WaterPumpStates::RUNNING) ///< If the weight is below the limit AND the pump is off
    {
      StartTotalWeight = BucketWeightSensor->getWeight(true) + WasteReservoirWeightSensor->getWeight(true);
      BucketPump->startPump();
      logToSerials(F("Watering"), true, 1);
    }
    if (*WasteLimit > 0 && WasteReservoirWeightSensor->getWeight(true) > *WasteLimit) //< Check if the waste reservoir is full
    {
      BucketPump->disablePump();
      logToSerials(F("Waste weight limit reached"), true, 1);
    }
  }
}

void HempyBucket::checkWateringFinished()
{
  BucketWeightSensor->readWeight(); ///< Force Bucket weight update 
  WasteReservoirWeightSensor->readWeight(); ///< Force Waste Reservoir weight update
 
  if (BucketWeightSensor->getWeight(false) > *StopWeight || BucketWeightSensor->getWeight(false) + WasteReservoirWeightSensor->getWeight(false) - StartTotalWeight > *StopWeight - *StartWeight) ///< If the weight is over the stop limit
  {
    BucketPump->stopPump();
    logToSerials("Stop weight reached", true, 0);    
  }
}

void HempyBucket::startWatering()
{  
  StartTotalWeight = BucketWeightSensor->getWeight(true)  + WasteReservoirWeightSensor->getWeight(true);
  BucketPump->startPump(true);
  logToSerials(F("Watering"), true, 1);
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

*/