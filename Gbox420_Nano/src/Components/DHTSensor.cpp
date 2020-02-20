#include "DHTSensor.h"

/*! 
 *  @brief  Class for the Digital Humidity and Temperature sensor
 */
DHTSensor::DHTSensor(const __FlashStringHelper *Name, Module *Parent, Settings::DHTSensorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Sensor = new DHT(*(&DefaultSettings->Pin), *(&DefaultSettings->Type));
  Sensor->begin(); ///dereference the pointer to the object and then call begin() on it. Same as (*Sensor).begin();
  Temp = new RollingAverage();
  Humidity = new RollingAverage();
  Parent->addToReportQueue(this);          ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Minute(this);  ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  logToSerials(F("DHT Sensor object created"), true, 1);
}

void DHTSensor::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  if (*Metric)
  {
    Temp->updateAverage(Sensor->readTemperature());
  }
  else
  {
    Temp->updateAverage(Sensor->readTemperature() * 1.8f + 32);
  }
  Humidity->updateAverage(Sensor->readHumidity());
}

void DHTSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Temp:"));
  strcat(LongMessage, getTempText(true, true)); ///Shows the average reading
  strcat_P(LongMessage, (PGM_P)F(" ; Humidity:"));
  strcat(LongMessage, getHumidityText(true, true));
  logToSerials(&LongMessage, true, 1);
}

float DHTSensor::getTemp(bool ReturnAverage)
{
  return Temp->getFloat(ReturnAverage);
}

char *DHTSensor::getTempText(bool IncludeUnits, bool ReturnAverage)
{
  if (IncludeUnits)
    return tempToText(Temp->getFloat(ReturnAverage));
  else
    return Temp->getFloatText(ReturnAverage);
}

float DHTSensor::getHumidity(bool ReturnAverage)
{
  return Humidity->getFloat(ReturnAverage);
}

char *DHTSensor::getHumidityText(bool IncludeUnits, bool ReturnAverage)
{
  if (IncludeUnits)
    return percentageToText(Humidity->getFloat(ReturnAverage));
  else
    return Humidity->getFloatText(ReturnAverage);
}