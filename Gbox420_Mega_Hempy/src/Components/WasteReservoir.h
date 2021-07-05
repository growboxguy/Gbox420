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
  bool setReserved(); ///< Returns true if the reservation is successful, false means the Waste Reservoir is busy measuring the run-off of another bucket
  bool getReserved();
  char *getReservedText(bool FriendlyFormat = false);
  void clearReservation(); ///< NEEDS TO BE CALLED at the end if the watering, makes the Waste Reservoir available
  bool getFull();
  char *getFullText(bool FriendlyFormat = false);

private:
protected:
  Module *Parent;
  WeightSensor *WasteWeightSensor; ///< Weight sensor to monitor the Hempy Bucket's waste reservoir, used to figure out when to stop watering
  float *WasteLimit;
  bool Full = false;     ///< If the WasteLimit is reached no further reservations are accepted
  bool Reserved = false; ///< Before a Hempy bucket can start watering it needs to reserve the waste reservoir. Only one bucket can be watering at one time, this ensures accurate run-off weight measurement.
  void checkLimit();    ///< Compare the weight to the WasteLimit
};