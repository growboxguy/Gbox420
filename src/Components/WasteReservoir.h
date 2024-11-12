#pragma once
#include "HX711.h"
#include "movingAvg.h"
#include "420Common.h"
#include "420Module.h"
#include "Sound.h"
#include "WeightSensor.h"

///< Weight sensor load cell
class WasteReservoir : virtual public Common
{
public:
  WasteReservoir(const __FlashStringHelper *Name, Module *Parent, Settings::WasteReservoirSettings *DefaultSettings, WeightSensor *WasteWeightSensor);
  void refresh_FiveSec();
  void report(bool FriendlyFormat = false);
  void updateState(WasteReservoirStates NewState);
  WasteReservoirStates getState();
  char *getStateText(bool FriendlyFormat = false); ///< Retuns the numerical or text state of the Hempy bucket
  bool setReserved(); ///< Returns true if the reservation is successful, false means the Waste Reservoir is busy measuring the run-off of another bucket
  void clearReservation(); ///< NEEDS TO BE CALLED at the end if the watering, makes the Waste Reservoir available
  void checkFull();        ///< Compares the waste reservoir weight against the WasteLimit. Sets Full flag once the limit is reached
  void setWasteLimit(float Weight);
  char *getWasteLimitText(bool FriendlyFormat = false);
  bool checkTarget(float DrainTargetWeight); ///<Check if the DrainTargetWeight is reached
  float getWeightIncrease();

private:
  uint32_t StateTimer = millis(); ///< Measures how much time is spent in a state

protected:
  Module *Parent;
  WeightSensor *WasteWeightSensor; ///< Weight sensor to monitor the Hempy Bucket's waste reservoir, used to figure out when to stop watering
  WasteReservoirStates State = WasteReservoirStates::IDLE;
  float &WasteLimit;
  float StartWeight = 0.0; ///< When a reservation is made the current waste reservoir weight is stored here
  void checkLimit();       ///< Compare the weight to the WasteLimit
};