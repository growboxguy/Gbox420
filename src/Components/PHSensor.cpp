#include "PHSensor.h"

PHSensor::PHSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PHSensorSettings *DefaultSettings) : Common(Name)
{ //constructor
  this->Parent = Parent;
  this->Pin = &DefaultSettings->Pin;
  this->Intercept = &DefaultSettings->Intercept;
  this->Slope = &DefaultSettings->Slope;
  pinMode(*Pin, INPUT);
  PH = new RollingAverage();
  Parent->AddToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  Parent->AddToRefreshQueue_Minute(this);  //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method 
  logToSerials(F("PHSensor object created"), true, 1);
}

void PHSensor::refresh_Minute()
{
  if (*DebugEnabled)
    Common::refresh_Minute();
  updatePH(false);
}

void PHSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("PH:"));
  strcat(LongMessage, PH->getFloatText(true));
  logToSerials(&LongMessage, true, 1);
}

float PHSensor::getPH(bool ReturnAverage)
{
  return PH->getFloat(ReturnAverage);
}

char *PHSensor::getPHText(bool ReturnAverage)
{
  return PH->getFloatText(ReturnAverage);
}

void PHSensor::updatePH(bool ShowRaw)
{
  int PHRaw = analogRead(*Pin);
  if (ShowRaw)
  {
    strncpy_P(LongMessage, (PGM_P)F("PH analog read: "), MaxTextLength);
    strcat(LongMessage, toText(PHRaw));
    Parent->addToLog(LongMessage);
  }
  PH->updateAverage((*Slope) * PHRaw + (*Intercept));
}

void PHSensor::setSlope(float Value)
{
  *Slope = Value;
  Parent->addToLog(F("PH slope updated"));
}

void PHSensor::setIntercept(float Value)
{
  *Intercept = Value;
  Parent->addToLog(F("PH intercept updated"));
}