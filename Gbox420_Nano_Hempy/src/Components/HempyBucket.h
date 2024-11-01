#pragma once
#include "420Common.h"
#include "420Module.h"
#include "Sound.h"
#include "WeightSensor.h"
#include "WasteReservoir.h"
#include "WaterPump.h"

///< Weight sensor measures the bucket weight and watering is based on the measured weight
enum WateringMode
{
  WEIGHT,
  TIMER,
  MANUAL
}; ///< Tracks what triggered the currently running watering

class HempyBucket : virtual public Common
{
public:
  HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings &DefaultSettings, WeightSensor &BucketWeightSensor, WasteReservoir &BucketWasteReservoir, WaterPump &BucketPump);
  void refresh_Sec();
  void refresh_FiveSec();
  void report(bool FriendlyFormat = false);
  void updateState(HempyStates NewState);
  HempyStates getState();
  char *getStateText(bool FriendlyFormat = false); ///< Retuns the numerical or text state of the Hempy bucket
  void disable();                                  ///< Disable watering logic  - Takes time to apply
  void disableRequest();                           ///< Signals to disable the watering logic at the next refresh - Runs fast
  void startWatering();                            ///< Turn on water pump, run until StopWeight is reached
  void startWateringRequest();                     ///< Turn on water pump, run until StopWeight is reached - Runs fast
  void stopWatering();                             ///< Turn off water pump
  void stopWateringRequest();                      ///< Turn off water pump - Runs fast
  void setEvaporationTarget(float Weight);         ///< Weight decrease between waterings
  char *getEvaporationTargetText(bool FriendlyFormat = false);
  void setOverflowTarget(float Weight);
  char *getOverflowTargetText(bool FriendlyFormat = false);
  void setDrainWaitTime(uint16_t Seconds);
  uint16_t getDrainWaitTime();
  char *getDrainWaitTimeText(bool FriendlyFormat = false);

  float getDryWeight();
  char *getDryWeightText(bool FriendlyFormat = false);
  void setDryWeight(float Weight);
  float getWetWeight();
  char *getWetWeightText(bool FriendlyFormat = false);
  void tareDryWetWeight();

private:
  uint32_t PumpOnTimer = millis();     ///< Used to measure pump's ON time in a cycle
  uint32_t WateringTime = 0;           ///< Measures total time the pump was ON during a watering (sum of multiple cycles)
  uint32_t StateTimer = millis();      ///< Measures how much time is spent in a state
  float BucketStartWeight = 0.0;       ///< Store the bucket start weight at each watering cycle
  bool DisableRequested = false;       ///< Signals to disable the watering logic
  bool StartWateringRequested = false; ///< Signals to start watering
  bool StopWateringRequested = false;  ///< Signals to stop watering

protected:
  Module *Parent;
  WeightSensor &BucketWeightSensor;     ///< Reference to the weight sensor
  WasteReservoir &BucketWasteReservoir; ///< Reference to the waste reservoir
  WaterPump &BucketPump;                ///< Reference to the water pump
  HempyStates State = HempyStates::IDLE;
  float &EvaporationTarget; ///< Reference to the evaporation target
  float &OverflowTarget;    ///< Reference to the overflow target
  float &InitialDryWeight;  ///< Reference to the initial dry weight
  uint16_t &DrainWaitTime;  ///< Reference to the drain wait time
  float DryWeight = 0.0;    ///< Bucket dry weight
  float WetWeight = 0.0;    ///< Bucket wet weight
};