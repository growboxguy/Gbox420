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
  void waterNow(bool UserRequest = false); ///< Turn on water pump, run until StopWeight is reached
  void checkRelay();                       ///< Sets the relay controlling the pump
  void checkWateringWeight();              ///< Watering based on weight
  void checkWateringFinished();            ///< Stops the pump when target weight or watering duration is reached
  void setStartWeight(float Weight);
  void setStopWeight(float Weight);
  void setWasteLimit(float Weight);
  void startWatering();
  float getStopWeight();
  float getStartWeight();
  float getWasteLimit();
  char *getStopWeightText(bool IncludeUnits);
  char *getStartWeightText(bool IncludeUnits);
  char *getWasteLimitText(bool IncludeUnits);

private:
  float StartTotalWeight; ///< Bucket + Waste reservoir combined weight, filled when starting the watering

protected:
  Module *Parent;
  WeightSensor *BucketWeightSensor;         ///< Weight sensor to monitor the Hempy Bucket's weight, used to figure out when to start and stop watering
  RollingAverage BucketWeight;
  WeightSensor *WasteReservoirWeightSensor; ///< Weight sensor to monitor the Hempy Bucket's waste reservoir, used to figure out when to stop watering
  RollingAverage WasteReservoirWeight;
  WaterPump *BucketPump;                    ///< Weight sensor to monitor the Hempy Bucket's weight, used to figure out when to start and stop watering
  float *StartWeight;                       ///< Start watering below this weight
  float *StopWeight;                        ///< Stop watering above this weight
  float *WasteLimit;                        ///< Waste reservoir full weight -> Pump gets disabled if reached
};