#include "PressureSensor.h"

PressureSensor::PressureSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PressureSensorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Pin = &DefaultSettings->Pin;
  Ratio = &DefaultSettings->Ratio;
  Offset = &DefaultSettings->Offset;
  AveragePressure = new movingAvg(MovingAverageDepth);
  AveragePressure.begin();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("Pressure Sensor object created"), true, 1);
}

void PressureSensor::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  readPressure();
}

void PressureSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("Pressure:"));
  strcat(LongMessage, getPressureText(false, true));
  strcat_P(LongMessage, (PGM_P)F(" ; Average:"));
  strcat(LongMessage, getPressureText(true, true));
  logToSerials(&LongMessage, true, 1);
}

float PressureSensor::readPressure(bool ReturnAverage)
{
  float Voltage = ((float)analogRead(*Pin)) * 5 / 1024;
  if (*Metric)
    Pressure = *Ratio * (Voltage - *Offset) * 1.0f; ///< unit: bar / 100kPa
  else
    Pressure = *Ratio * (Voltage - *Offset) * 14.5038f; ///< unit: PSI
  AveragePressure->reading(Pressure * 100);
  if (ReturnAverage)
  {
    return AveragePressure->getAvg() / 100.0;
  }
  else
  {
    return Pressure;
  }
}

float PressureSensor::getPressure(bool ReturnAverage)
{
  if (ReturnAverage)
    return AveragePressure->getAvg() / 100.0;
  else
    return Pressure;
}

char *PressureSensor::getPressureText(bool ReturnAverage, bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_pressure(getPressure(ReturnAverage));
  else
    return toText(getPressure(ReturnAverage));
}

void PressureSensor::readOffset()
{ ///< Should only be called when there is 0 pressure
  float sum = 0;
  for (uint8_t i = 0; i < 50; i++)
  {
    sum += analogRead(*Pin);
    delay(10);
  }
  float AeroOffsetRecommendation = (sum / 50) * 5 / 1024; ///< Reads voltage at 0 pressure
  strncpy_P(LongMessage, (PGM_P)F("0 pressure Offset: "), MaxWordLength);
  strcat(LongMessage, toText(AeroOffsetRecommendation));
  Parent->addToLog(LongMessage);
}

void PressureSensor::setOffset(float Value)
{
  *Offset = Value;
  AveragePressure->reset();
  Parent->addToLog(F("Offset updated"));
}

void PressureSensor::setRatio(float Value)
{
  *Ratio = Value;
  AveragePressure->reset();
  Parent->addToLog(F("Ratio updated"));
}