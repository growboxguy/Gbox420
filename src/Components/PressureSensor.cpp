#include "PressureSensor.h"

PressureSensor::PressureSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PressureSensorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Pin = &DefaultSettings->Pin;
  Ratio = &DefaultSettings->Ratio;
  Offset = &DefaultSettings->Offset;
  AveragePressure = new movingAvg(MovingAverageDepth);
  AveragePressure->begin();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("PressureSensor ready"), true, 3);
}

void PressureSensor::report(bool JSONReport)
{
  Common::report(JSONReport); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  if (JSONReport)             //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    strcat_P(LongMessage, (PGM_P)F("\"P\":\""));
    strcat(LongMessage, getPressureText(false, false));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else //Print a report to the Serial console
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("Pressure:"));
    strcat(LongMessage, getPressureText(false, true));
    logToSerials(&LongMessage, true, 1);
  }
}

void PressureSensor::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  readPressure();
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
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("Offset: "));
  strcat(ShortMessage, toText(AeroOffsetRecommendation));
  Parent->addToLog(ShortMessage);
}

void PressureSensor::setOffset(float Value)
{
  *Offset = Value;
  AveragePressure->reset();
  // appendName(true);
  // strcat_P(ShortMessage, (PGM_P)F("offset updated"));
  // Parent->addToLog(ShortMessage);
}

void PressureSensor::setRatio(float Value)
{
  *Ratio = Value;
  AveragePressure->reset();
  // appendName(true);
  // strcat_P(ShortMessage, (PGM_P)F("ratio updated"));
  // Parent->addToLog(ShortMessage);
}