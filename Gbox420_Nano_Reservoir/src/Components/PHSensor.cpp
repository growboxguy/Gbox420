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
  logToSerials(F("PHSensor object created"), true, 1);
}

void PHSensor::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  updatePH(false);
}

void PHSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("PH:"));
  strcat(LongMessage, getPHText(false));
  strcat_P(LongMessage, (PGM_P)F(" ; Average:"));
  strcat(LongMessage, getPHText(true));
  logToSerials(&LongMessage, true, 1);
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
  Parent->addToLog(F("PH slope updated"));
}

void PHSensor::setIntercept(float Value)
{
  *Intercept = Value;
  AveragePH->reset();
  Parent->addToLog(F("PH intercept updated"));
}