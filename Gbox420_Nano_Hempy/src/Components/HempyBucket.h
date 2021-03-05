#pragma once
#include "420Common.h"
#include "420Module.h"
#include "Sound.h"
#include "WeightSensor.h"
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
  HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WeightSensor *WasteReservoirWeightSensor, WaterPump *BucketPump);
  void refresh_Sec();
  void refresh_FiveSec();
  void report();
  void updateState(HempyStates NewState);
  HempyStates getState();
  void disable();
  void startWatering(); ///< Turn on water pump, run until StopWeight is reached
  void stopWatering();  ///< Turn on water pump, run until StopWeight is reached
  void setEvaporationTarget(float Weight);
  void setOverflowTarget(float Weight);
  void setDrainWaitTime(uint16_t Seconds);
  void setWasteLimit(float Weight);
  float getDryWeight();
  void setDryWeight(float Weight);
  float getWetWeight();
  void tareDryWetWeight();

private:
  float StartTotalWeight;                ///< Bucket + Waste reservoir combined weight, filled when starting the watering
  uint32_t PumpOnTimer = millis();     ///< Used to measure pump's ON time in a cycle
  uint32_t WateringTime = 0;     ///< Measures total time the pump was ON during a watering (sum of multiple cycles)
  uint32_t StateTimer = millis();        ///< Measures how much time is spent in a state
  float WasteReservoirStartWeight = 0.0; ///< Store the waste reservoir weight at the start of watering
  float BucketStartWeight = 0.0;         ///< Store the bucket start weight at each watering cycle

protected:
  Module *Parent;
  WeightSensor *BucketWeightSensor;         ///< Weight sensor to monitor the Hempy Bucket's weight, used to figure out when to start and stop watering
  WeightSensor *WasteReservoirWeightSensor; ///< Weight sensor to monitor the Hempy Bucket's waste reservoir, used to figure out when to stop watering
  WaterPump *BucketPump;                    ///< Weight sensor to monitor the Hempy Bucket's weight, used to figure out when to start and stop watering
  HempyStates State = HempyStates::IDLE;
  float *EvaporationTarget;  ///< (kg/lbs) Amount of water that should evaporate before starting the watering cycles
  float *OverflowTarget;     ///< (kg/lbs) Amount of water that should go to the waste reservoir after a watering cycle
  float *WasteLimit;         ///< Waste reservoir full weight -> Pump gets disabled if reached
  float *InitialDryWeight;   ///When the module starts up the Hempy Bucket's wet weight is unknown, use this weight as the first watering weight
  uint16_t *DrainWaitTime;   ///< (sec) How long to wait after watering for the water to drain
  float DryWeight = 0.0;     ///< (kg/lbs) If Bucket weight drops below this: Start the watering cycles. Automatically calculated from the wet bucket weight minus the EvaportationTarget
  float WetWeight = 0.0;     ///< (kg/lbs) Bucket weight after watering, measured after watering timeout is expired.
};