#include "WeightSensor.h"

WeightSensor::WeightSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WeightSensorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  AverageWeight = new movingAvg(MovingAverageDepth);
  AverageWeight->begin();
  Scale = &DefaultSettings->Scale;
  Offset = &DefaultSettings->Offset;
  Sensor = new HX711();
  Sensor->begin(*(&DefaultSettings->DTPin), *(&DefaultSettings->SCKPin));
  Sensor->set_scale(*Scale);
  Sensor->set_offset(*Offset);
  Parent->addToReportQueue(this);
  readWeight();
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("WeightSensor ready"), true, 3);
}

void WeightSensor::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  if (FriendlyFormat)             //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    strcat_P(LongMessage, (PGM_P)F("\"WT\":\""));
    strcat(LongMessage, getWeightText(false, false));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else                                             //Print a report to the Serial console
  {                                                ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("Weight:"));
    strcat(LongMessage, getWeightText(false, true));
    logToSerials(&LongMessage, true, 1);
  }
}

void WeightSensor::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  if (TareRequested)
  {
    TareRequested = false;
    tare();
  }
  if (CalibrateRequested)
  {
    CalibrateRequested = false;
    calibrate();
  }
  readWeight();
}

float WeightSensor::readWeight(bool ReturnAverage)
{
  if (Sensor->wait_ready_timeout(500))
  {
    Weight = Sensor->get_units();
    //if(Weight<0)
    //  {Weight = 0.0;}  ///< Zero out negative weight
    AverageWeight->reading(Weight * 100); ///< AverageWeight is integer based to save memory, multipy by 100 to store the first two decimal digits
  }
  if (ReturnAverage)
  {
    return AverageWeight->getAvg() / 100.0; ///< Divide by floating point 100 to regain the first two decimal digits
  }
  else
  {
    return Weight;
  }
}

float WeightSensor::getWeight(bool ReturnAverage)
{
  if (ReturnAverage)
    return AverageWeight->getAvg() / 100.0;
  else
    return Weight;
}

char *WeightSensor::getWeightText(bool ReturnAverage, bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_weight(getWeight(ReturnAverage));
  }
  else
  {
    return toText(getWeight(ReturnAverage));
  }
}

void WeightSensor::triggerTare()
{
  TareRequested = true;
}

void WeightSensor::tare() ///< Time intense, cannot be called straight from the website. Response would time out.
{
  Sensor->tare();
  *Offset = Sensor->get_offset();
  AverageWeight->reset();
  // appendName(true);
  // strcat_P(ShortMessage, (PGM_P)F("tared"));
  // Parent->addToLog(ShortMessage);
  Parent->getSoundObject()->playOnSound();
}

void WeightSensor::triggerCalibration(int CalibrationWeight)
{
  this->CalibrationWeight = CalibrationWeight;
  CalibrateRequested = true;
}

void WeightSensor::calibrate() ///< Time intense, cannot be called straight from the website. Response would time out.
{
  *Scale = (float)Sensor->get_value() / CalibrationWeight;
  Sensor->set_scale(*Scale);
  AverageWeight->reset();
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("calibrated"));
  Parent->addToLog(ShortMessage);
  Parent->getSoundObject()->playOnSound();
}

void WeightSensor::setScale(float NewScale)
{
  *Scale = NewScale;
  Sensor->set_scale(*Scale);
}