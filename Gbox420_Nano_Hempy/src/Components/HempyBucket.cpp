#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WeightSensor *WasteReservoirWeightSensor, WaterPump *BucketPump) : Common(Name)
{
  this->Parent = Parent;
  this->BucketWeightSensor = BucketWeightSensor;
  this->BucketPump = BucketPump;
  this->WasteReservoirWeightSensor = WasteReservoirWeightSensor;
  StartWeight = &DefaultSettings->StartWeight;
  StopWeight = &DefaultSettings->StopWeight;
  WasteLimit = &DefaultSettings->WasteLimit;
  //BucketWeight = new RollingAverage();
  //WasteReservoirWeight = new RollingAverage();
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
}

void HempyBucket::refresh_Sec()
{
  if (BucketPump->getOnState())
  {
    logToSerials("Watering.....");
    checkWateringFinished(); ///< When pump is on check if it is time to stop
  }
}

void HempyBucket::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("Start weight:"));
  strcat(LongMessage, toText_weight(*StartWeight));
  strcat_P(LongMessage, (PGM_P)F(" ; Stop weight:"));
  strcat(LongMessage, toText_weight(*StopWeight));
  strcat_P(LongMessage, (PGM_P)F(" ; Waste limit:"));
  strcat(LongMessage, toText_weight(*WasteLimit));
  logToSerials(&LongMessage, true, 1);
}

void HempyBucket::checkWateringWeight()
{
  float AverageBucketWeight = BucketWeightSensor->readWeight(); // BucketWeight->updateAverage(BucketWeightSensor->readWeight());  
  float AverageWasteReservoirWeight = WasteReservoirWeightSensor->readWeight();// WasteReservoirWeight->updateAverage(WasteReservoirWeightSensor->readWeight()); 
    
  logToSerials(AverageBucketWeight, true, 0);
  logToSerials(AverageWasteReservoirWeight, true, 0);

  if (BucketPump->getEnabledState()) ///< If the weight based watering is enabled AND the pump is enabled
  {
    if (AverageBucketWeight < *StartWeight && !BucketPump->getOnState()) ///< If the weight is below the limit AND the pump is off
    {
      StartTotalWeight = AverageBucketWeight + AverageWasteReservoirWeight;
      BucketPump->startPump();
      logToSerials(F("Watering..."), true, 1);
    }
    if (*WasteLimit > 0 && AverageWasteReservoirWeight > *WasteLimit) //< Check if the waste reservoir is full
    {
      BucketPump->disablePump();
      logToSerials(F("Waste weight limit reached"), true, 1);
    }
  }
}

void HempyBucket::checkWateringFinished()
{
  float CurrentBucketWeight = BucketWeightSensor->readWeight();  
  float CurrentWasteReservoirWeight = WasteReservoirWeightSensor->readWeight(); 
    logToSerials("Current");
  logToSerials(CurrentBucketWeight, true, 0);
  logToSerials(CurrentWasteReservoirWeight, true, 0);
  
  if (CurrentBucketWeight > *StopWeight || CurrentBucketWeight + CurrentWasteReservoirWeight - StartTotalWeight > *StopWeight - *StartWeight) ///< If the weight is over the stop limit
  {
    BucketPump->stopPump();
    logToSerials("Stop weight reached", true, 0);    
  }
}

void HempyBucket::startWatering()
{
  float AverageBucketWeight = BucketWeightSensor->readWeight(); //BucketWeight->updateAverage(BucketWeightSensor->readWeight());  
  float AverageWasteReservoirWeight = WasteReservoirWeightSensor->readWeight(); // WasteReservoirWeight->updateAverage(WasteReservoirWeightSensor->readWeight()); 
  StartTotalWeight = AverageBucketWeight + AverageWasteReservoirWeight;
  BucketPump->startPump(true);
  logToSerials(F("Watering..."), true, 1);
}

float HempyBucket::getStartWeight()
{
  return *StartWeight;
}

float HempyBucket::getStopWeight()
{
  return *StopWeight;
}

float HempyBucket::getWasteLimit()
{
  return *WasteLimit;
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

char *HempyBucket::getWasteLimitText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_weight(*WasteLimit);
  else
    return toText(*WasteLimit);
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