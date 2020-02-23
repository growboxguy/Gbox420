#include "WeightSensor.h"

WeightSensor::WeightSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WeightSensorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Scale = &DefaultSettings->Scale;
  TareOffset = &DefaultSettings->TareOffset;
  Weight = new RollingAverage();
  Sensor = new HX711();
  Sensor -> begin(*(&DefaultSettings->DTPin), *(&DefaultSettings->SCKPin));
  Sensor -> set_scale(*Scale);
  Sensor -> set_offset(*TareOffset);  
  Parent->addToReportQueue(this);         ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_FiveSec(this); ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  Parent->addToRefreshQueue_Sec(this);  
  logToSerials(F("Weight Sensor object created"), true, 1);
}

void WeightSensor::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  if(TareRequested){
    TareRequested = false;
    tare();
  }
  if(CalibrateRequested){
    CalibrateRequested = false;
    calibrate();
  }
  readWeight();
}

void WeightSensor::refresh_Sec()
{
   
}

void WeightSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Weight:"));
  strcat(LongMessage, getWeightText(true, false));
  logToSerials(&LongMessage, true, 1);
}

void WeightSensor::readWeight(){
  Weight -> updateAverage(Sensor -> get_units());
}

float WeightSensor::getWeight(bool ReturnAverage)
{
  return Weight->getFloat(ReturnAverage);
}

char *WeightSensor::getWeightText(bool IncludeUnits, bool ReturnAverage)
{
  if (IncludeUnits)
    return weightToText(Weight->getFloat(ReturnAverage));
  else
    return Weight->getFloatText(ReturnAverage);
}

void WeightSensor::triggerTare(){
  TareRequested = true;
  Parent->addToLog(F("Updating tare...")); ///This can take up to 1 minute, when the component is next refreshed
}

void WeightSensor::tare() ///Time intense, cannot be called straight from the website. Response would time out.
{
  Sensor -> tare();
  *TareOffset = Sensor -> get_offset();
  Weight->resetAverage();
  Parent->addToLog(F("Tare updated"));
}

void WeightSensor::triggerCalibration(int CalibrationWeight){
  this -> CalibrationWeight = CalibrationWeight;
  CalibrateRequested = true;
  Parent->addToLog(F("Calibrating weight..")); ///This can take up to 1 minute, when the component is next refreshed
}

void WeightSensor::calibrate() ///Time intense, cannot be called straight from the website. Response would time out.
{
  *Scale = (float) Sensor -> get_value() / CalibrationWeight;
  Sensor -> set_scale(*Scale);
  Weight->resetAverage();
  Parent->addToLog(F("Weight calibrated"));
}

void WeightSensor::setScale(float NewScale)
{
  *Scale = NewScale;
  Sensor -> set_scale(*Scale);
}