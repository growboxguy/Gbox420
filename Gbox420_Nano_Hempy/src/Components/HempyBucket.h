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
  void waterNow(bool UserRequest = false){}; ///< Turn on water pump, run until StopWeight is reached
  //void checkRelay();                       ///< Sets the relay controlling the pump
  //void checkWateringWeight();              ///< Watering based on weight
  //void checkWateringFinished();            ///< Stops the pump when target weight or watering duration is reached
  //void setStartWeight(float Weight);
  //void setStopWeight(float Weight);  
 // void startWatering();
  //float getStopWeight();
  //float getStartWeight();
  // char *getStopWeightText(bool IncludeUnits);
  //char *getStartWeightText(bool IncludeUnits);
  void setWasteLimit(float Weight);
  float getWasteLimit();
  char *getWasteLimitText(bool IncludeUnits);

private:
  float StartTotalWeight; ///< Bucket + Waste reservoir combined weight, filled when starting the watering

protected:
  Module *Parent;
  WeightSensor *BucketWeightSensor;         ///< Weight sensor to monitor the Hempy Bucket's weight, used to figure out when to start and stop watering
  WeightSensor *WasteReservoirWeightSensor; ///< Weight sensor to monitor the Hempy Bucket's waste reservoir, used to figure out when to stop watering
  WaterPump *BucketPump;                    ///< Weight sensor to monitor the Hempy Bucket's weight, used to figure out when to start and stop watering
  HempyStates State = HempyStates::IDLE;
  float EvaporationTarget = 3; ///< (kg/lbs) Amount of water that should evaporate before starting the watering cycles
  float OverflowTarget = 0.3;  ///< (kg/lbs) Amount of water that should go to the waste reservoir after a watering cycle
  float NextWateringWeight = 0.0; ///< (kg/lbs) If Bucket weight drops below this: Start the watering cycles. Automatically calculated from the wet bucket weight minus the EvaportationTarget
  float WasteReservoirStartWeight = 0.0;   ///< Store the waste reservoir weight at the start of watering
  float BucketStartWeight;  ///< Store the bucket start weight at each watering cycle
  uint16_t DrainWaitTime = 60; ///< (sec) How long to wait after watering for the water to drain
  uint16_t WateringTimeout = 820; ///< (sec) Maximum time the watering can take (including all Watering-Draining cycles). If reached the Hempy bucket will get disabled
  uint32_t WateringTimer = millis(); ///< Measures total watering time
  uint32_t StateTimer = millis(); ///< Measures how much time is spent in a state
 
  float *WasteLimit;                        ///< Waste reservoir full weight -> Pump gets disabled if reached
};