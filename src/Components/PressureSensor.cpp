#include "PressureSensor.h"

PressureSensor::PressureSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PressureSensorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Pin = &DefaultSettings->Pin;
  Ratio = &DefaultSettings->Ratio;
  Offset = &DefaultSettings->Offset;
  Pressure = new RollingAverage();
  Parent->addToReportQueue(this);         //Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Minute(this); //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  logToSerials(F("Pressure Sensor object created"), true, 1);
}

void PressureSensor::refresh_Minute()
{
  if (*DebugEnabled)
    Common::refresh_Minute();
  readPressure();
}

void PressureSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("Pressure:"));
  strcat(LongMessage, getPressureText(true, true));
  logToSerials(&LongMessage, true, 1);
}

void PressureSensor::readPressure()
{
  float Voltage = ((float)analogRead(*Pin)) * 5 / 1024;
  if (*MetricSystemEnabled)
    Pressure->updateAverage(*Ratio * (Voltage - *Offset) * 1.0f); // unit: bar / 100kPa
  else
    Pressure->updateAverage(*Ratio * (Voltage - *Offset) * 14.5038f); //unit: PSI
}

float PressureSensor::getPressure(bool ReturnAverage)
{
  return Pressure->getFloat(ReturnAverage);
}

char *PressureSensor::getPressureText(bool IncludeUnits, bool ReturnAverage)
{
  if (IncludeUnits)
    return pressureToText(Pressure->getFloat(ReturnAverage));
  else
    return Pressure->getFloatText(ReturnAverage);
}

void PressureSensor::readOffset()
{ //Should only be called when there is 0 pressure
  float sum = 0;
  for (byte i = 0; i < 50; i++)
  {
    sum += analogRead(*Pin);
    delay(10);
  }
  float AeroOffsetRecommendation = (sum / 50) * 5 / 1024; //Reads voltage at 0 pressure
  strncpy_P(LongMessage, (PGM_P)F("0 pressure Offset: "), MaxTextLength);
  strcat(LongMessage, toText(AeroOffsetRecommendation));
  Parent->addToLog(LongMessage);
}

void PressureSensor::setOffset(float Value)
{
  *Offset = Value;
  Pressure->resetAverage();
  Parent->addToLog(F("Pressure offset updated"));
}

void PressureSensor::setRatio(float Value)
{
  *Ratio = Value;
  Pressure->resetAverage();
  Parent->addToLog(F("Pressure/voltage ratio updated"));
}