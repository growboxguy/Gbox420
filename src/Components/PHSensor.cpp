#include "PHSensor.h"

PHSensor::PHSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PHSensorSettings *DefaultSettings) : Common(Name)
{ ///< constructor
  this->Parent = Parent;
  this->Pin = &DefaultSettings->Pin;
  this->Intercept = &DefaultSettings->Intercept;
  this->Slope = &DefaultSettings->Slope;
  pinMode(*Pin, INPUT);
  AveragePH = new movingAvg(MovingAverageDepth);
  AveragePH->begin();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("PHSensor ready"), true, 3);
}

void PHSensor::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  updatePH(false);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void PHSensor::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"P\":\""));
  strcat(LongMessage, getPHText(true));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void PHSensor::updatePH(bool ShowRaw)
{
  int PHRaw = analogRead(*Pin);
  if (ShowRaw)
  {
    strncpy_P(LongMessage, (PGM_P)F("PH analog read: "), MaxWordLength);
    strcat(LongMessage, toText(PHRaw));
    Parent->addToLog(LongMessage);
  }
  PH = (*Slope) * PHRaw + (*Intercept);
  AveragePH->reading(PH * 100);
}

float PHSensor::getPH(bool ReturnAverage)
{
  if (ReturnAverage)
    return AveragePH->getAvg() / 100.0;
  else
    return PH;
}

char *PHSensor::getPHText(bool ReturnAverage)
{
  return toText(getPH(ReturnAverage));
}

void PHSensor::setSlope(float Value)
{
  *Slope = Value;
  AveragePH->reset();
  Parent->addToLog(getName(F("slope updated")));
  Parent->addToLog(ShortMessage);
}

void PHSensor::setIntercept(float Value)
{
  *Intercept = Value;
  AveragePH->reset();
  Parent->addToLog(getName(F("intercept updated")));
  Parent->addToLog(ShortMessage);
}