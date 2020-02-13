#include "WaterTempSensor.h"

WaterTempSensor::WaterTempSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WaterTempSensorSettings *DefaultSettings) : Common(Name)
{ //constructor
  this->Parent = Parent;
  //pinMode(*Pin, INPUT);
  Temp = new RollingAverage();
  TempSensorWire = new OneWire(*(&DefaultSettings->Pin)); //Reservoir waterproof temperature sensor (DS18B20)
  TempSensor = new DallasTemperature(TempSensorWire);     //Reservoir waterproof temperature sensor (DS18B20)
  TempSensor->begin();
  Parent->addToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  logToSerials(F("WaterTempSensor object created"), true, 1);
}

void WaterTempSensor::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  TempSensor->requestTemperatures();
  if (*MetricSystemEnabled)
  {
    Temp->updateAverage(TempSensor->getTempCByIndex(0));
  }
  else
  {
    Temp->updateAverage(TempSensor->getTempFByIndex(0));
  }
}

void WaterTempSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("Temp:"));
  strcat(LongMessage, getTempText(true, true));
  logToSerials(&LongMessage, true, 1);
}

float WaterTempSensor::getTemp(bool ReturnAverage)
{
  return Temp->getFloat(ReturnAverage);
}

char *WaterTempSensor::getTempText(bool IncludeUnits, bool ReturnAverage)
{
  if (IncludeUnits)
    return tempToText(Temp->getFloat(ReturnAverage));
  else
    return Temp->getFloatText(ReturnAverage);
}