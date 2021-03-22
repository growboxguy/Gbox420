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
  logToSerials(F("WeightSensor object created"), true, 3);
}

void WeightSensor::refresh_FiveSec()
{
  if (*Debug)
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

void WeightSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("Weight:"));
  strcat(LongMessage, getWeightText(false, true));
  strcat_P(LongMessage, (PGM_P)F(" ; Average:"));
  strcat(LongMessage, getWeightText(true, true));
  logToSerials(&LongMessage, true, 1);
}

void WeightSensor::reportToJSON(__attribute__((unused)) bool CloseJSON)
{
  //Common::reportToJSON(LongMessage); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\""));
  strcat_P(LongMessage, (PGM_P)Name);
  strcat_P(LongMessage, (PGM_P)F("\":{"));
  strcat_P(LongMessage, (PGM_P)F("\"WT\":\""));
  strcat(LongMessage, getWeightText(false, true));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
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

char *WeightSensor::getWeightText(bool ReturnAverage, bool IncludeUnits)
{
  if (IncludeUnits)
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
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("updating tare"));
  Parent->addToLog(ShortMessage);
}

void WeightSensor::tare() ///< Time intense, cannot be called straight from the website. Response would time out.
{
  Sensor->tare();
  *Offset = Sensor->get_offset();
  AverageWeight->reset();
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("tare updated"));
  Parent->addToLog(ShortMessage);
  Parent->getSoundObject()->playOnSound();
}

void WeightSensor::triggerCalibration(int CalibrationWeight)
{
  this->CalibrationWeight = CalibrationWeight;
  CalibrateRequested = true;
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("calibrating"));
  Parent->addToLog(ShortMessage);
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