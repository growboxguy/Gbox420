#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WeightSensor *WasteReservoirWeightSensor, WaterPump *BucketPump) : Common(Name)
{
  this->Parent = Parent;
  this->BucketWeightSensor = BucketWeightSensor;
  this->BucketPump = BucketPump;
  this->WasteReservoirWeightSensor = WasteReservoirWeightSensor;
  EvaporationTarget = &DefaultSettings->EvaporationTarget;
  OverflowTarget = &DefaultSettings->OverflowTarget;
  WasteLimit = &DefaultSettings->WasteLimit;
  InitialDryWeight = &DefaultSettings->InitialDryWeight;
  DryWeight = DefaultSettings->InitialDryWeight;
  DrainWaitTime = &DefaultSettings->DrainWaitTime;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("HempyBucket ready"), true, 3);
}

void HempyBucket::report(bool JSONReport)
{
  Common::report(JSONReport); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  { ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
    strcat(LongMessage, toText((int)getState()));
    strcat_P(LongMessage, (PGM_P)F("\",\"DW\":\""));
    strcat(LongMessage, toText(DryWeight));
    strcat_P(LongMessage, (PGM_P)F("\",\"WW\":\""));
    strcat(LongMessage, toText(WetWeight));
    strcat_P(LongMessage, (PGM_P)F("\",\"ET\":\""));
    strcat(LongMessage, toText(*EvaporationTarget));
    strcat_P(LongMessage, (PGM_P)F("\",\"OF\":\""));
    strcat(LongMessage, toText(*OverflowTarget));
    strcat_P(LongMessage, (PGM_P)F("\",\"WL\":\""));
    strcat(LongMessage, toText(*WasteLimit));
    strcat_P(LongMessage, (PGM_P)F("\",\"DT\":\""));
    strcat(LongMessage, toText(*DrainWaitTime));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else //Print a report to the Serial console
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("State:"));
    strcat(LongMessage, toText_hempyState(State));
    strcat_P(LongMessage, (PGM_P)F(" ; DryWeight:"));
    strcat(LongMessage, toText_weight(DryWeight));
    strcat_P(LongMessage, (PGM_P)F(" ; WetWeight:"));
    strcat(LongMessage, toText_weight(WetWeight));
    strcat_P(LongMessage, (PGM_P)F(" ; Evaporation:"));
    strcat(LongMessage, toText_weight(*EvaporationTarget));
    strcat_P(LongMessage, (PGM_P)F(" ; OverFlow:"));
    strcat(LongMessage, toText_weight(*OverflowTarget));
    strcat_P(LongMessage, (PGM_P)F(" ; WasteLimit:"));
    strcat(LongMessage, toText_weight(*WasteLimit));
    strcat_P(LongMessage, (PGM_P)F(" ; DrainTimeOut:"));
    strcat(LongMessage, toText_second(*DrainWaitTime));
    logToSerials(&LongMessage, true, 1);
  }
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
  if (State == HempyStates::WATERING || State == HempyStates::DRAINING)
  {
    updateState(State);
  }
}

void HempyBucket::updateState(HempyStates NewState)
{
  bool BlockOverWritingState = false; //Used when a state transitions to a new state
  if (State != NewState)
  {
    StateTimer = millis();                         ///< Start measuring the time spent in the new State
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)Name);
    strcat_P(LongMessage, (PGM_P)F(" state: "));
    strcat(LongMessage, toText_hempyState(State));
    strcat_P(LongMessage, (PGM_P)F(" -> "));
    strcat(LongMessage, toText_hempyState(NewState));
    logToSerials(&LongMessage, true, 3);
  }

  BucketWeightSensor->readWeight(false);         ///< Force Bucket weight update
  WasteReservoirWeightSensor->readWeight(false); ///< Force Waste Reservoir weight update

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
        updateState(HempyStates::WATERING);
        BlockOverWritingState = true;
      }
    }
    break;
  case HempyStates::WATERING:
    if (State != NewState)
    {
      BucketStartWeight = BucketWeightSensor->getWeight();              // Store the bucket weight before starting the pump
      if (State == HempyStates::IDLE || State == HempyStates::DISABLED) // First time entering the WATERING-DRIAINING cycles
      {
        WasteReservoirStartWeight = WasteReservoirWeightSensor->getWeight(); //Store the waste reservoir weight, watering will stop once WasteReservoirStartWeight + OverflowTarget is reached
        PumpOnTimer = millis();                                              ///Start measuring the pump ON time for this cycle
        WateringTime = 0;                                                    ///Reset the counter that tracks the total pump ON time during the watering process (multiple WATERING-DRAINING cycles)
      }
      if (State == HempyStates::DRAINING) /// The WATERING-DRIAINING cycles are already in progress
      {
        PumpOnTimer = millis(); ///Start measuring the pump ON time for this cycle
      }
      BucketPump->startPump(true);
    }
    if (BucketWeightSensor->getWeight(false) >= WetWeight && BucketWeightSensor->getWeight(false) - BucketStartWeight + WasteReservoirWeightSensor->getWeight() - WasteReservoirStartWeight >= *OverflowTarget) //Wet weight reached AND Target overflow's worth of water was added, wait for it to drain
    {
      WateringTime += millis() - PumpOnTimer;
      updateState(HempyStates::DRAINING);
      BlockOverWritingState = true;
    }
    if (WateringTime > ((uint32_t)BucketPump->getPumpTimeOut() * 1000) || BucketPump->getState() == WaterPumpStates::DISABLED || WasteReservoirWeightSensor->getWeight() >= *WasteLimit) ///< Timeout before the waste target was reached
    {
      updateState(HempyStates::DISABLED);
      BlockOverWritingState = true;
    }
    break;
  case HempyStates::DRAINING:
    BucketPump->stopPump();
    State = HempyStates::DRAINING;                                 //Store the new state immediately - Only important when DrainWaitTime is set to 0
    if (millis() - StateTimer > ((uint32_t)*DrainWaitTime * 1000)) ///< Waiting for the water to drain
    {
      if (WasteReservoirWeightSensor->getWeight(false) - WasteReservoirStartWeight >= *OverflowTarget) //Check if target overflow weight is reached
      {
        WetWeight = BucketWeightSensor->getWeight(); //Measure wet weight
        // DryWeight = ((float)((int)(WetWeight - *EvaporationTarget * 10))) / 10; //Calculate next watering weight
        DryWeight = WetWeight - *EvaporationTarget; //Calculate next watering weight
        updateState(HempyStates::IDLE);
      }
      else
      {
        if (WasteReservoirWeightSensor->getWeight() >= *WasteLimit) ///Safety feature: Disable pump if waste reservoir full
        {
          updateState(HempyStates::DISABLED);
        }
        else
        {
          updateState(HempyStates::WATERING); /// Continue watering
        }
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

void HempyBucket::disable()
{
  Parent->getSoundObject()->playOffSound();
  updateState(HempyStates::DISABLED);
}

void HempyBucket::startWatering()
{
  Parent->getSoundObject()->playOnSound();
  updateState(HempyStates::WATERING);
}

void HempyBucket::stopWatering()
{
  Parent->getSoundObject()->playOnSound();
  updateState(HempyStates::IDLE);
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

void HempyBucket::setOverflowTarget(float Weight)
{
  if (*OverflowTarget != Weight)
  {
    *OverflowTarget = Weight;
    Parent->getSoundObject()->playOnSound();
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

void HempyBucket::setWasteLimit(float Weight)
{
  if (*WasteLimit != Weight)
  {
    *WasteLimit = Weight;
    Parent->getSoundObject()->playOnSound();
  }
}

HempyStates HempyBucket::getState()
{
  return State;
}

float HempyBucket::getDryWeight()
{
  return DryWeight;
}

void HempyBucket::setDryWeight(float Weight)
{
  if (!isnan(Weight) && DryWeight != Weight)
  {
    DryWeight = Weight;
    *InitialDryWeight = Weight; //Store the value in EEPROM
    Parent->getSoundObject()->playOnSound();
  }
}

float HempyBucket::getWetWeight()
{
  return WetWeight;
}

void HempyBucket::tareDryWetWeight()
{
  DryWeight = *InitialDryWeight;
  WetWeight = 0.0;
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("Dry/Wet tared"));
  logToSerials(&ShortMessage, true, 3);
  Parent->getSoundObject()->playOnSound();
}