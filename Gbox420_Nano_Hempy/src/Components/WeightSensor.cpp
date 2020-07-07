#include "WeightSensor.h"

WeightSensor::WeightSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WeightSensorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Weight = 0.0;
  Scale = &DefaultSettings->Scale;
  Offset = &DefaultSettings->Offset;
  Sensor = new HX711();
  Sensor -> begin(*(&DefaultSettings->DTPin), *(&DefaultSettings->SCKPin));
  Sensor -> set_scale(*Scale);
  Sensor -> set_offset(*Offset);  
  Parent->addToReportQueue(this);         
  Parent->addToRefreshQueue_FiveSec(this); 
  Parent->addToRefreshQueue_Sec(this);
  logToSerials(F("WeightSensor object created"), true, 1);
}

void WeightSensor::refresh_FiveSec()
{
 
}

void WeightSensor::refresh_Sec()
{
    if (*Debug)
    Common::refresh_Sec();
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

void WeightSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Weight:"));
  strcat(LongMessage, getWeightText(true));
  logToSerials(&LongMessage, true, 1);
}

void WeightSensor::readWeight(){
  if (Sensor -> wait_ready_timeout(200)) {
    Weight = Sensor -> get_units();
  }
}

float WeightSensor::getWeight()
{
  return Weight;
}

char *WeightSensor::getWeightText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_weight(Weight);
  else
    return toText(Weight);
}

void WeightSensor::triggerTare(){
  TareRequested = true;
  Parent->addToLog(F("Updating tare...")); ///This can take up to 1 minute, when the component is next refreshed
}

void WeightSensor::tare() ///Time intense, cannot be called straight from the website. Response would time out.
{
  Sensor -> tare();
  *Offset = Sensor -> get_offset();
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
  Parent->addToLog(F("Weight calibrated"));
}

void WeightSensor::setScale(float NewScale)
{
  *Scale = NewScale;
  Sensor -> set_scale(*Scale);
}