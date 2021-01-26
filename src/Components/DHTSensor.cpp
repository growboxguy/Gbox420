#include "DHTSensor.h"

/*! 
 *  @brief  Class for the Digital Humidity and Temperature sensor
 */
DHTSensor::DHTSensor(const __FlashStringHelper *Name, Module *Parent, Settings::DHTSensorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Sensor = new DHT(DefaultSettings->Pin, DefaultSettings->Type);
  Sensor->begin(); ///< dereference the pointer to the object and then call begin() on it. Same as (*Sensor).begin();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("DHTSensor object created"), true, 3);
}

void DHTSensor::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  readSensor();
}

void DHTSensor::readSensor()
{
  if (*Metric)
  {
    Temp = Sensor->readTemperature();
  }
  else
  {
    Temp = Sensor->readTemperature() * 1.8f + 32;
  }
  Humidity = Sensor->readHumidity();
}

void DHTSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("Temp:"));
  strcat(LongMessage, getTempText(true)); ///< Shows the average reading
  strcat_P(LongMessage, (PGM_P)F(" ; Humidity:"));
  strcat(LongMessage, getHumidityText(true));
  logToSerials(&LongMessage, true, 1);
}

float DHTSensor::getTemp()
{
  return Temp;
}

char *DHTSensor::getTempText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_temp(Temp);
  else
    return toText(Temp);
}

float DHTSensor::getHumidity()
{
  return Humidity;
}

char *DHTSensor::getHumidityText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_percentage(Humidity);
  else
    return toText(Humidity);
}