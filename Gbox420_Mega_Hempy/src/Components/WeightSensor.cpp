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

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void WeightSensor::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"W\":\""));
  strcat(LongMessage, getWeightText(false, FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void WeightSensor::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  if (TareRequested)
  {
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
    AverageWeight->reading(Weight * 100); ///< AverageWeight is integer based to save memory, multiply by 100 to store the first two decimal digits
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

void WeightSensor::tareRequest()
{
  TareRequested = true;  
  Parent->getSoundObject()->playOnSound();
}

void WeightSensor::tare() ///< Time intense, cannot be called straight from the website. Response would time out.
{
  TareRequested = false; ///< Clear the flag requesting a tare
  Sensor->tare();
  *Offset = Sensor->get_offset();
  AverageWeight->reset();
  strcpy(ShortMessage, getName(F("offset ")));
  sprintf(ShortMessage + strlen(ShortMessage), "%ld", *Offset);
  Parent->addToLog(ShortMessage);
}

void WeightSensor::triggerCalibration(float KnownWeight)
{
  this->KnownWeight = KnownWeight;
  CalibrateRequested = true;  
  Parent->getSoundObject()->playOnSound();
}

void WeightSensor::calibrate() ///< Time intense, cannot be called straight from the website. Response would time out.
{
  char LogEntry[MaxShotTextLength] = "";  
  *Scale = Sensor->get_value() / KnownWeight; 
  Sensor->set_scale(*Scale);
  AverageWeight->reset();
  strcpy(LogEntry, getName(F("scale ")));
  strcat(LogEntry, toText(*Scale));  
  Parent->addToLog(LogEntry);
}

void WeightSensor::setScale(float NewScale)
{
  *Scale = NewScale;
  Sensor->set_scale(*Scale);
}