#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings &DefaultSettings, WeightSensor &BucketWeightSensor, WaterPump &BucketPump)
    : Common(Name),
      Parent(Parent),
      BucketWeightSensor(BucketWeightSensor),
      BucketPump(BucketPump),
      DisabledState(DefaultSettings.DisabledState),
      EvaporationTarget(DefaultSettings.EvaporationTarget),
      DrainTargetWeight(DefaultSettings.DrainTargetWeight),
      WateringIncrement(DefaultSettings.WateringIncrement),
      StartWeight(DefaultSettings.StartWeight),
      MaxWeight(DefaultSettings.MaxWeight),
      DrainWaitTime(DefaultSettings.DrainWaitTime)
{
  DryWeight = DefaultSettings.StartWeight; // Until first watering use StartWeight. After watering DryWeight is calculated from WetWeight - EvaporationTarget
  WetWeight = DryWeight + DefaultSettings.EvaporationTarget;
  if (DisabledState)
  {
    State = HempyStates::DISABLED;
  }
  else
  {
    State = HempyStates::IDLE;
  }
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
  HempyStates TargetState = NewState;
  bool ProcessUpdate = true;

  while (ProcessUpdate)
  {
    ProcessUpdate = false;                        ///< Ensure the update is processed only once, even if the state changes multiple times during the function executionBucketWeightSensor.readWeight();              ///< Force Bucket weight update
    StateWeight = BucketWeightSensor.readWeight(); // Store the bucket weight before
    bool ChangeDetected = (State != TargetState); ///< Detect if the state is changing

    if (ChangeDetected)
    {
      memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable,  then print exaple Bucket1 state: DRAINING -> IDLE
      strcat(LongMessage, getName(F("state: ")));
      strcat(LongMessage, toText_hempyState(State));
      strcat_P(LongMessage, (PGM_P)F(" -> "));
      strcat(LongMessage, toText_hempyState(TargetState));
      logToSerials(&LongMessage, true, 3);

      State = TargetState;
      StateTimer = millis();
      DisabledState = (State == HempyStates::DISABLED);
    }

    switch (State)
    {
    case HempyStates::DISABLED:
      if (ChangeDetected)
        BucketPump.disablePump();
      break;
    case HempyStates::DRY:
      if (ChangeDetected)
        BucketPump.stopPump(true);
      if (StateWeight >= WetWeight) ///< If the bucket has been refilled manually, go back to IDLE
      {
        TargetState = HempyStates::IDLE;
        ProcessUpdate = true;
      }
      break;
    case HempyStates::IDLE:
      if (ChangeDetected)
        BucketPump.stopPump(true);
      if (BucketPump.getState() != WaterPumpStates::DISABLED && StateWeight <= DryWeight)
      {
        TargetState = HempyStates::WATERING;
        ProcessUpdate = true;
      }
      break;
    case HempyStates::WATERING:
      if (ChangeDetected)
      {
        WateringStartWeight = StateWeight; // Store the weight at the start of the watering cycle
        PumpOnTimer = millis();            /// Start measuring the pump ON time for this cycle
        BucketPump.startPump(true);
      }

      // Calculate cumulative time pump has been running
      uint32_t TotaltWateringTime = WateringTime + (millis() - PumpOnTimer);

      // 1. Check for Target Weight Increment Reached
      if (StateWeight >= WateringStartWeight + WateringIncrement) ///< Target overflow's worth of water was added, wait for it to drain
      {
        WateringTime = TotaltWateringTime;
        TargetState = HempyStates::DRAINING;
        ProcessUpdate = true;
      }
      // 2. Check for Timeout or Pump Failure
      else if ((TotaltWateringTime > ((uint32_t)WateringTimeLimit * 1000) || BucketPump.getState() == WaterPumpStates::DISABLED)) ///< Watering failed if: Timeout or pump failed
      {
        TargetState = HempyStates::DRY;
        ProcessUpdate = true;
      }
      // 3. Check for Max Weight safety limit
      else if (StateWeight >= MaxWeight) // the maximum weight was reached without reaching drain target
      {
        TargetState = HempyStates::IDLE;
        ProcessUpdate = true;
      }
      break;
    case HempyStates::DRAINING:
      if (ChangeDetected)
      {
        BucketPump.stopPump();
        if (DrainStartWeight == 0.0)
        {
          DrainStartWeight = StateWeight; // Store the bucket weight at the start of the draining cycle, used to detect when the water has been drained
        }
      }
      if (millis() - StateTimer > ((uint32_t)DrainWaitTime * 1000)) ///< Waiting for the water to drain
      {
        /*if(Debug)
        {
          logToSerials(F("Drained:"), false, 3);
          logToSerials(DrainProgress,false, 1);
          logToSerials(F("/"), false, 0);
          logToSerials(DrainTargetWeight,true, 0);
        }
        */
        if (DrainStartWeight - StateWeight >= DrainTargetWeight) // Check if target overflow weight is reached -> Watering completed
        {
          WetWeight = BucketWeightSensor.getWeight(); ///< Measure the current weight of the bucket (wet weight)
          DryWeight = WetWeight - EvaporationTarget;  ///< Calculate the next watering weight
          if (DryWeight < StartWeight)                ///< Ensure the calculated DryWeight is not less than the user-configured StartWeight
            DryWeight = StartWeight;                  ///< Use the StartWeight from the UI as the minimum threshold
          DrainStartWeight = 0.0;                     ///< Reset the drain start weight for the next cycle
          WateringTime = 0;                           ///< Reset the watering time for the next cycle
          TargetState = HempyStates::IDLE;
        }
        else
        {
          TargetState = HempyStates::WATERING; ///< Continue watering
        }
        ProcessUpdate = true;
      }
      break;
    }
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