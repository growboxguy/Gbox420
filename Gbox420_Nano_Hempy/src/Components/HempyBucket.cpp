#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WasteReservoir *BucketWasteReservoir, WaterPump *BucketPump) : Common(Name)
{
  this->Parent = Parent;
  this->BucketWeightSensor = BucketWeightSensor;
  this->BucketPump = BucketPump;
  this->BucketWeightSensor = BucketWeightSensor;
  this->BucketWasteReservoir = BucketWasteReservoir;
  EvaporationTarget = &DefaultSettings->EvaporationTarget;
  OverflowTarget = &DefaultSettings->OverflowTarget;
  InitialDryWeight = &DefaultSettings->InitialDryWeight;
  DryWeight = DefaultSettings->InitialDryWeight;
  WetWeight = DryWeight + *EvaporationTarget - *OverflowTarget; //Initial value will get re-calculated after a watering
  DrainWaitTime = &DefaultSettings->DrainWaitTime;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("Hempy bucket ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void HempyBucket::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, getStateText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"DW\":\""));
  strcat(LongMessage, getDryWeightText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"WW\":\""));
  strcat(LongMessage, getWetWeightText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"ET\":\""));
  strcat(LongMessage, getEvaporationTargetText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"OF\":\""));
  strcat(LongMessage, getOverflowTargetText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"DT\":\""));
  strcat(LongMessage, getDrainWaitTimeText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void HempyBucket::refresh_Sec()
{
  Common::refresh_Sec();
  if (State == HempyStates::WATERING || State == HempyStates::DRAINING)
  {
    updateState(State);
  }
  if (DisableRequested)
  {
    DisableRequested = false;
    disable();
  }
  if (StartWateringRequested)
  {
    StartWateringRequested = false;
    startWatering();
  }
  if (StopWateringRequested)
  {
    StopWateringRequested = false;
    stopWatering();
  }
}

void HempyBucket::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  updateState(State);
}

void HempyBucket::updateState(HempyStates NewState)
{
  bool BlockOverWritingState = false; //Used when a state transitions to a new state
  if (State != NewState)
  {
    StateTimer = millis();                         ///< Start measuring the time spent in the new State
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat(LongMessage, getName(F("state: ")));
    strcat(LongMessage, toText_hempyState(State));
    strcat_P(LongMessage, (PGM_P)F(" -> "));
    strcat(LongMessage, toText_hempyState(NewState));
    logToSerials(&LongMessage, true, 3);
  }

  BucketWeightSensor->readWeight(false); ///< Force Bucket weight update

  switch (NewState)
  {
  case HempyStates::DISABLED:
    BucketPump->disablePump();
    break;
  case HempyStates::IDLE:
    BucketPump->stopPump(true);
    if (BucketPump->getState() != WaterPumpStates::DISABLED && (DryWeight <= 0 || BucketWeightSensor->getWeight() <= DryWeight))
    {
      if (BucketWeightSensor->getWeight() >= DryWeight - *OverflowTarget) ///< Filters out waterings triggered by a disconnected weight sensor
      {
        if (BucketWasteReservoir->setReserved())
        {
          updateState(HempyStates::WATERING);
          BlockOverWritingState = true;
        }
      }
    }
    break;
  case HempyStates::WATERING:
    if (State != NewState)
    {
      BucketStartWeight = BucketWeightSensor->getWeight();              // Store the bucket weight before starting the pump
      if (State == HempyStates::IDLE || State == HempyStates::DISABLED) // First time entering the WATERING-DRIAINING cycles
      {
        PumpOnTimer = millis(); ///Start measuring the pump ON time for this cycle
        WateringTime = 0;       ///Reset the counter that tracks the total pump ON time during the watering process (multiple WATERING-DRAINING cycles)
      }
      if (State == HempyStates::DRAINING) /// The WATERING-DRIAINING cycles are already in progress
      {
        PumpOnTimer = millis(); ///Start measuring the pump ON time for this cycle
      }
      BucketPump->startPump(true);
    }
    if (BucketWeightSensor->getWeight(false) >= WetWeight && BucketWeightSensor->getWeight(false) - BucketStartWeight + BucketWasteReservoir->getWeightIncrease() >= *OverflowTarget) //Wet weight reached AND Target overflow's worth of water was added, wait for it to drain
    {
      WateringTime += millis() - PumpOnTimer;
      updateState(HempyStates::DRAINING);
      BlockOverWritingState = true;
    }
    if (WateringTime > ((uint32_t)BucketPump->getTimeOut() * 1000) || BucketPump->getState() == WaterPumpStates::DISABLED || BucketWasteReservoir->getState() == WasteReservoirStates::FULL) ///< Disable watering if: Timeout before the waste target was reached, pump failed or the waste reservoir got full
    {
      BucketWasteReservoir->clearReservation();
      updateState(HempyStates::DISABLED);
      BlockOverWritingState = true;
    }
    break;
  case HempyStates::DRAINING:
    BucketPump->stopPump();
    State = HempyStates::DRAINING;                                 //Store the new state immediately - Only important when DrainWaitTime is set to 0
    if (millis() - StateTimer > ((uint32_t)*DrainWaitTime * 1000)) ///< Waiting for the water to drain
    {
      if (BucketWasteReservoir->checkTarget(*OverflowTarget)) //Check if target overflow weight is reached
      {
        WetWeight = BucketWeightSensor->getWeight(); //Measure wet weight
        DryWeight = WetWeight - *EvaporationTarget;  //Calculate next watering weight
        BucketWasteReservoir->clearReservation();    ///< Free up the waste reservoir
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

void HempyBucket::disable() //Takes time, do not call directly from an interupt (ESP-link website would timeout)
{
  if (State == HempyStates::DRAINING || State == HempyStates::WATERING)
    BucketWasteReservoir->clearReservation();
  updateState(HempyStates::DISABLED);
  Parent->addToLog(getName(getStateText(true)));
  Parent->getSoundObject()->playOffSound();
}

void HempyBucket::disableRequest() //Stores the request only, will apply the next time the Hempy Bucket is refreshing
{
  DisableRequested = true;
}

void HempyBucket::startWatering()
{
  if (BucketWasteReservoir->setReserved())
  {
    Parent->addToLog(getName(F("watering")));
    updateState(HempyStates::WATERING);
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    Parent->addToLog(BucketWasteReservoir->getName(F("busy")));
    updateState(HempyStates::IDLE);
  }
}

void HempyBucket::startWateringRequest() //Stores the request only, will apply the next time the Hempy Bucket is refreshing
{
  StartWateringRequested = true;
}

void HempyBucket::stopWatering()
{
  if (State == HempyStates::DRAINING || State == HempyStates::WATERING)
    BucketWasteReservoir->clearReservation();
  updateState(HempyStates::IDLE);
  Parent->addToLog(getName(F("stopped")));
  Parent->getSoundObject()->playOnSound();
}

void HempyBucket::stopWateringRequest() //Stores the request only, will apply the next time the Hempy Bucket is refreshing
{
  StopWateringRequested = true;
}

void HempyBucket::setEvaporationTarget(float Weight)
{
  if (*EvaporationTarget != Weight)
  {
    if (DryWeight > 0) //If the next watering weight is already known
    {
      DryWeight += *EvaporationTarget - Weight;
    }
    *EvaporationTarget = Weight;
    Parent->getSoundObject()->playOnSound();
  }
}

char *HempyBucket::getEvaporationTargetText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_weight(*EvaporationTarget);
  }
  else
  {
    return toText(*EvaporationTarget);
  }
}

void HempyBucket::setOverflowTarget(float Weight)
{
  if (*OverflowTarget != Weight)
  {
    *OverflowTarget = Weight;
    Parent->getSoundObject()->playOnSound();
  }
}

char *HempyBucket::getOverflowTargetText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_weight(*OverflowTarget);
  }
  else
  {
    return toText(*OverflowTarget);
  }
}

void HempyBucket::setDrainWaitTime(uint16_t Seconds)
{
  if (*DrainWaitTime != Seconds)
  {
    *DrainWaitTime = Seconds;
    Parent->getSoundObject()->playOnSound();
  }
}

uint16_t HempyBucket::getDrainWaitTime()
{
  return *DrainWaitTime;
}

char *HempyBucket::getDrainWaitTimeText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_second(*DrainWaitTime);
  }
  else
  {
    return toText(*DrainWaitTime);
  }
}

HempyStates HempyBucket::getState()
{
  return State;
}

char *HempyBucket::getStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_hempyState(State);
  }
  else
  {
    return toText((int)State);
  }
}

void HempyBucket::setDryWeight(float Weight)
{
  if (!isnan(Weight) && DryWeight != Weight)
  {
    DryWeight = Weight;
    WetWeight = DryWeight + *EvaporationTarget; ///< Calculate initial wet weight based on the evaporation target. Updated to the actual wet weight at the next watering
    *InitialDryWeight = Weight;                 //Store the value in EEPROM
    Parent->getSoundObject()->playOnSound();
  }
}

float HempyBucket::getDryWeight()
{
  return DryWeight;
}

char *HempyBucket::getDryWeightText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_weight(DryWeight);
  }
  else
  {
    return toText(DryWeight);
  }
}

float HempyBucket::getWetWeight()
{
  return WetWeight;
}

char *HempyBucket::getWetWeightText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_weight(WetWeight);
  }
  else
  {
    return toText(WetWeight);
  }
}

void HempyBucket::tareDryWetWeight()
{
  DryWeight = *InitialDryWeight;
  WetWeight = DryWeight + *EvaporationTarget - *OverflowTarget;
  logToSerials(getName(F("Dry/Wet tared")), true, 3);
  Parent->getSoundObject()->playOnSound();
}