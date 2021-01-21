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
  void checkWateringTimer();               ///< Watering based on timer
  void checkWateringFinished();            ///< Stops the pump when target weight or watering duration is reached
  void setWeightBasedWatering(bool State);
  void setStartWeight(float Weight);
  void setStopWeight(float Weight);
  void setWasteLimit(float Weight);
  void setTimerBasedWatering(bool State);
  void setWateringInterval(uint16_t Minutes);
  void setWateringDuration(uint16_t Seconds);
  void startWatering();
  float getStopWeight();
  float getStartWeight();
  float getWasteLimit();
  char *getStopWeightText(bool IncludeUnits);
  char *getStartWeightText(bool IncludeUnits);
  char *getWasteLimitText(bool IncludeUnits);

private:
  WateringMode WateringTrigger;
  float StartTotalWeight; ///< Bucket + Waste reservoir combined weight, filled when starting the watering

protected:
  Module *Parent;
  bool *TimerBasedWatering;                 ///< Enables timed waterings to prevent the soil from drying out with seedlings
  uint16_t *WateringInterval;               ///< How often to water in MINUTES
  uint16_t *WateringDuration;               ///< How long to water in SECONDS
  uint32_t WateringTimer = millis();        ///< last time the plants were watered
  WeightSensor *BucketWeightSensor;         ///< Weight sensor to monitor the Hempy Bucket's weight, used to figure out when to start and stop watering
  RollingAverage BucketWeight;              ///< Average bucket weight based on the last few readings, MovingAverageDepth in Settings.h defines how many readings to calculate from
  WeightSensor *WasteReservoirWeightSensor; ///< Weight sensor to monitor the Hempy Bucket's waste reservoir, used to figure out when to stop watering
  RollingAverage WasteReservoirWeight;      ///< Average waste reservoir weight based on the last few readings, MovingAverageDepth in Settings.h defines how many readings to calculate from
  WaterPump *BucketPump;                    ///< Weight sensor to monitor the Hempy Bucket's weight, used to figure out when to start and stop watering
  bool *WeightBasedWatering;                ///< Enables weight based waterings
  float *StartWeight;                       ///< Start watering below this weight
  float *StopWeight;                        ///< Stop watering above this weight
  float *WasteLimit;                        ///< Waste reservoir full weight -> Pump gets disabled if reached
};