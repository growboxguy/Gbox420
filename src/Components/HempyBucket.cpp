#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings &DefaultSettings, WeightSensor &BucketWeightSensor, WaterPump &BucketPump)
    : Common(Name),
      Parent(Parent),
      BucketWeightSensor(BucketWeightSensor),
      BucketPump(BucketPump),
      EvaporationTarget(DefaultSettings.EvaporationTarget),
      DrainTargetWeight(DefaultSettings.DrainTargetWeight),
      WateringIncrement(DefaultSettings.WateringIncrement),
      StartWeight(DefaultSettings.StartWeight),
      MaxWeight(DefaultSettings.MaxWeight),
      DrainWaitTime(DefaultSettings.DrainWaitTime)
{
  WetWeight = DefaultSettings.MaxWeight;  // Initialize WetWeight to MaxWeight until first watering
  DryWeight = DefaultSettings.StartWeight; // Until first watering use StartWeight. After watering DryWeight is calculated from WetWeight - EvaporationTarget
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
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, getDryWeightText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"W\":\""));
  strcat(LongMessage, getWetWeightText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"M\":\""));
  strcat(LongMessage, getMaxWeightText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"WI\":\""));
  strcat(LongMessage, getWateringIncrementText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"ET\":\""));
  strcat(LongMessage, getEvaporationTargetText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"DT\":\""));
  strcat(LongMessage, getDrainTargetWeightText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"DW\":\""));
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
  bool BlockOverWritingState = false; // Used when a state transitions to a new state
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

  BucketWeightSensor.readWeight(); ///< Force Bucket weight update

  switch (NewState)
  {
  case HempyStates::DISABLED:
    BucketPump.disablePump();
    break;
  case HempyStates::IDLE:
    BucketPump.stopPump(true);
    if (BucketPump.getState() != WaterPumpStates::DISABLED && BucketWeightSensor.getWeight() <= DryWeight)
    {
      updateState(HempyStates::WATERING);
      BlockOverWritingState = true;
    }
    break;
  case HempyStates::WATERING:
    if (State != NewState)
    {
      StateWeight = BucketWeightSensor.getWeight();                     // Store the bucket weight before starting the pump
      if (State == HempyStates::IDLE || State == HempyStates::DISABLED) // First time entering the WATERING-DRIAINING cycles
      {
        PumpOnTimer = millis(); /// Start measuring the pump ON time for this cycle
        WateringTime = 0;       /// Reset the counter that tracks the total pump ON time during the watering process (multiple WATERING-DRAINING cycles)
      }
      if (State == HempyStates::DRAINING) /// The WATERING-DRIAINING cycles are already in progress
      {
        PumpOnTimer = millis(); /// Start measuring the pump ON time for this cycle
      }
      BucketPump.startPump(true);
    }
    if (BucketWeightSensor.getWeight() >= StateWeight + WateringIncrement && BucketWeightSensor.getWeight() >= WetWeight) ///< Wet weight reached AND Target overflow's worth of water was added, wait for it to drain
    {
      WateringTime += millis() - PumpOnTimer;
      updateState(HempyStates::DRAINING);
      BlockOverWritingState = true;
    }
    if (WateringTime > ((uint32_t)BucketPump.getTimeOut() * 1000) || BucketPump.getState() == WaterPumpStates::DISABLED || BucketWeightSensor.getWeight() > MaxWeight) ///< Disable watering if: Timeout before the waste target was reached, pump failed or the maximum weight was reached
    {
      updateState(HempyStates::DISABLED);
      BlockOverWritingState = true;
    }
    break;
  case HempyStates::DRAINING:
    BucketPump.stopPump();
    if (State != NewState)
    {
      StateWeight = BucketWeightSensor.getWeight();
    }
    if (millis() - StateTimer > ((uint32_t)DrainWaitTime * 1000)) ///< Waiting for the water to drain
    {
      DrainProgress += StateWeight - BucketWeightSensor.getWeight(); ///<  Calculate how much water has drained
      /*if(Debug)
      {
        logToSerials(F("Drained:"), false, 3);
        logToSerials(DrainProgress,false, 1);
        logToSerials(F("/"), false, 0);
        logToSerials(DrainTargetWeight,true, 0);
      }
      */
      if (DrainProgress >= DrainTargetWeight) // Check if target overflow weight is reached -> Watering completed
      {
        WetWeight = BucketWeightSensor.getWeight(); ///< Measure the current weight of the bucket (wet weight)
        DryWeight = WetWeight - EvaporationTarget;  ///< Calculate the next watering weight
        if (DryWeight < StartWeight)                ///< Ensure the calculated DryWeight is not less than the user-configured StartWeight
          DryWeight = StartWeight;                  ///< Use the StartWeight from the UI as the minimum threshold
        DrainProgress = 0;                          ///< Reset the drain progress tracker
        updateState(HempyStates::IDLE);
      }
      else
      {
        updateState(HempyStates::WATERING); ///< Continue watering
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

void HempyBucket::disable() ///< Takes time, do not call directly from an interrupt (ESP-link website would timeout)
{
  if (State != HempyStates::DISABLED)
  {
    updateState(HempyStates::DISABLED);
    Parent->addToLog(getName(getStateText(true)));
    Parent->getSoundObject()->playOffSound();
  }
}

void HempyBucket::disableRequest() // Stores the request only, will apply the next time the Hempy Bucket is refreshing
{
  DisableRequested = true;
}

void HempyBucket::startWatering()
{
  if (State != HempyStates::WATERING && State != HempyStates::DRAINING)
  {
    Parent->addToLog(getName(F("watering")));
    updateState(HempyStates::WATERING);
    Parent->getSoundObject()->playOnSound();
  }
}

void HempyBucket::startWateringRequest() // Stores the request only, will apply the next time the Hempy Bucket is refreshing
{
  StartWateringRequested = true;
}

void HempyBucket::stopWatering()
{
  if (State != HempyStates::IDLE)
  {
    updateState(HempyStates::IDLE);
    Parent->addToLog(getName(F("stopped")));
    Parent->getSoundObject()->playOnSound();
  }
}

void HempyBucket::stopWateringRequest() // Stores the request only, will apply the next time the Hempy Bucket is refreshing
{
  StopWateringRequested = true;
}

void HempyBucket::setMaxWeight(float Weight)
{
  if (MaxWeight != Weight)
  {
    MaxWeight = Weight;
    Parent->getSoundObject()->playOnSound();
  }
}

void HempyBucket::setEvaporationTarget(float newEvaporationTarget)
{
  if (EvaporationTarget != newEvaporationTarget)
  {
    DryWeight = max(StartWeight, WetWeight - newEvaporationTarget);
    EvaporationTarget = newEvaporationTarget;
    Parent->getSoundObject()->playOnSound();
  }
}

char *HempyBucket::getEvaporationTargetText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_weight(EvaporationTarget);
  }
  else
  {
    return toText(EvaporationTarget);
  }
}

void HempyBucket::setDrainTargetWeight(float Weight)
{
  if (DrainTargetWeight != Weight)
  {
    DrainTargetWeight = Weight;
    Parent->getSoundObject()->playOnSound();
  }
}

char *HempyBucket::getDrainTargetWeightText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_weight(DrainTargetWeight);
  }
  else
  {
    return toText(DrainTargetWeight);
  }
}

void HempyBucket::setDrainWaitTime(uint16_t Seconds)
{
  if (DrainWaitTime != Seconds)
  {
    DrainWaitTime = Seconds;
    Parent->getSoundObject()->playOnSound();
  }
}

uint16_t HempyBucket::getDrainWaitTime()
{
  return DrainWaitTime;
}

char *HempyBucket::getDrainWaitTimeText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_second(DrainWaitTime);
  }
  else
  {
    return toText(DrainWaitTime);
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

void HempyBucket::setStartWeight(float Weight)
{
  if (StartWeight != Weight)
  {
    DryWeight = Weight;   // Reset dry weight - next watering will re-calculate it (WetWeight-EvaporationTarget)
    StartWeight = Weight; // Store the value in EEPROM
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

char *HempyBucket::getMaxWeightText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_weight(MaxWeight);
  }
  else
  {
    return toText(MaxWeight);
  }
}

void HempyBucket::setWateringIncrement(float Weight)
{
  if (WateringIncrement != Weight)
  {
    WateringIncrement = Weight;
    Parent->getSoundObject()->playOnSound();
  }
}

float HempyBucket::getWateringIncrement()
{
  return WateringIncrement;
}

char *HempyBucket::getWateringIncrementText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_weight(WateringIncrement);
  }
  else
  {
    return toText(WateringIncrement);
  }
}

void HempyBucket::tareDryWetWeight()
{
  DryWeight = StartWeight;
  WetWeight = 0.0; // Will be measured at the next watering
  logToSerials(getName(F("Dry/Wet tared")), true, 3);
  Parent->getSoundObject()->playOnSound();
}