#include "PressureSensor.h"

PressureSensor::PressureSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PressureSensorSettings *DefaultSettings) 
    : Common(Name), Parent(Parent),
      Pin(DefaultSettings->Pin),
      Offset(DefaultSettings->Offset),
      Ratio(DefaultSettings->Ratio)
{
  AveragePressure = new movingAvg(MovingAverageDepth);
  AveragePressure->begin();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("PressureSensor ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void PressureSensor::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"P\":\""));
  strcat(LongMessage, getPressureText(false, FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void PressureSensor::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  readPressure();
}

float PressureSensor::readPressure(bool ReturnAverage)
{
  float Voltage = ((float)analogRead(Pin)) * 5 / 1024;
  if (Metric)
    Pressure = Ratio * (Voltage - Offset) * 1.0f; ///< unit: bar / 100kPa
  else
    Pressure = Ratio * (Voltage - Offset) * 14.5038f; ///< unit: PSI
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

char *PressureSensor::getPressureText(bool ReturnAverage, bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_pressure(getPressure(ReturnAverage));
  else
    return toText(getPressure(ReturnAverage));
}

void PressureSensor::readOffset()
{ ///< Should only be called when there is 0 pressure
  float sum = 0;
  for (uint8_t i = 0; i < 50; i++)
  {
    sum += analogRead(Pin);
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
  Offset = Value;
  AveragePressure->reset();
  Parent->addToLog(getName(F("offset updated")));
}

void PressureSensor::setRatio(float Value)
{
  Ratio = Value;
  AveragePressure->reset();
  Parent->addToLog(getName(F("ratio updated")));
}