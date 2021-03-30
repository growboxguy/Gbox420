#include "WaterTempSensor.h"

WaterTempSensor::WaterTempSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WaterTempSensorSettings *DefaultSettings) : Common(Name)
{ ///< constructor
  this->Parent = Parent;
  ///< pinMode(*Pin, INPUT);
  Temp = 0.0;
  TempSensorWire = new OneWire(*(&DefaultSettings->Pin)); ///< Reservoir waterproof temperature sensor (DS18B20)
  TempSensor = new DallasTemperature(TempSensorWire);     ///< Reservoir waterproof temperature sensor (DS18B20)
  TempSensor->begin();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("WaterTempSensor ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void WaterTempSensor::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"T\":\""));
  strcat(LongMessage, getTempText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void WaterTempSensor::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  readSensor();
}

void WaterTempSensor::readSensor()
{
  TempSensor->requestTemperatures();
  if (*Metric)
  {
    Temp = TempSensor->getTempCByIndex(0);
  }
  else
  {
    Temp = TempSensor->getTempFByIndex(0);
  }
}

float WaterTempSensor::getTemp()
{
  return Temp;
}

char *WaterTempSensor::getTempText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_temp(Temp);
  else
    return toText(Temp);
}