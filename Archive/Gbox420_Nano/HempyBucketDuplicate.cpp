#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeight) : Common(Name)
{
  this->Parent = Parent;
  this->BucketWeightSensor = BucketWeightSensor;
  
  StartWateringWeight = &DefaultSettings->StartWateringWeight;
  StopWateringWeight = &DefaultSettings->StopWateringWeight;
  PumpPin = &DefaultSettings->PumpPin;
  PumpEnabled = &DefaultSettings->PumpEnabled;
  PumpTimeout = &DefaultSettings->PumpTimeout;

  Parent->addToReportQueue(this);         
  Parent->addToRefreshQueue_FiveSec(this); 
  Parent->addToRefreshQueue_Sec(this);
  logToSerials(F("Hempy Bucket object created"), true, 1);
}

//struct HempyBucketSettings HempyB1 = {.StartWateringWeight = 4.9,.StopWateringWeight = 6.5, .PumpEnabled = true, .PumpTimeout = 120,};  ///Default settings for the Hempy Module
//struct HempyBucketSettings HempyB2 = {.StartWateringWeight = 4.9,.StopWateringWeight = 6.5, .PumpEnabled = true, .PumpTimeout = 120,};  ///Default settings for the Hempy Module
  

void HempyBucket::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec(); 
}

void HempyBucket::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec(); 
}

void HempyBucket::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Weight:"));
  strcat(LongMessage, BucketWeightSensor -> getWeightText());
  strcat_P(LongMessage, (PGM_P)F(" ["));
  strcat(LongMessage, toText(*StartWateringWeight));
  strcat_P(LongMessage, (PGM_P)F("/"));
  strcat(LongMessage, toText(*StopWateringWeight));
  strcat_P(LongMessage, (PGM_P)F("], Pump:"));
  strcat_P(LongMessage, toText_pumpState(*PumpEnabled,PumpOn));
  logToSerials(&LongMessage, true, 1);
}

void HempyBucket::checkWeight(){
  if (!PumpOn && BucketWeightSensor -> getWeight < StartWateringWeight) {
    PumpOn = true;
  }
  digitalWrite(PumpPin,PumpOn);
}

/*
float HempyBucket::getWeight()
{
  return Weight;
}

char *HempyBucket::getWeightText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_weight(Weight);
  else
    return toText(Weight);
}

void HempyBucket::triggerTare(){
  TareRequested = true;
  Parent->addToLog(F("Updating tare...")); ///This can take up to 1 minute, when the component is next refreshed
}

void HempyBucket::tare() ///Time intense, cannot be called straight from the website. Response would time out.
{
  Sensor -> tare();
  *TareOffset = Sensor -> get_offset();
  Parent->addToLog(F("Tare updated"));
}

void HempyBucket::triggerCalibration(int CalibrationWeight){
  this -> CalibrationWeight = CalibrationWeight;
  CalibrateRequested = true;
  Parent->addToLog(F("Calibrating weight..")); ///This can take up to 1 minute, when the component is next refreshed
}

void HempyBucket::calibrate() ///Time intense, cannot be called straight from the website. Response would time out.
{
  *Scale = (float) Sensor -> get_value() / CalibrationWeight;
  Sensor -> set_scale(*Scale);
  Parent->addToLog(F("Weight calibrated"));
}

void HempyBucket::setScale(float NewScale)
{
  *Scale = NewScale;
  Sensor -> set_scale(*Scale);
} */