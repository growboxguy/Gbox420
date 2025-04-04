#include "DHTSensor.h"

/*!
 *  @brief  Class for the Digital Humidity and Temperature sensor
 */
DHTSensor::DHTSensor(const __FlashStringHelper *Name, Module *Parent, Settings::DHTSensorSettings *DefaultSettings) : Common(Name), Parent(Parent)
{
  Sensor = new DHT(DefaultSettings->Pin, DefaultSettings->Type);
  Sensor->begin(); ///< dereference the pointer to the object and then call begin() on it. Same as (*Sensor).begin();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("DHTSensor ready"), true, 3);
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void DHTSensor::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"T\":\""));
  strcat(LongMessage, getTempText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"H\":\""));
  strcat(LongMessage, getHumidityText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void DHTSensor::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  readSensor();
}

void DHTSensor::readSensor()
{
  if (Metric)
  {
    Temp = Sensor->readTemperature();
  }
  else
  {
    Temp = Sensor->readTemperature() * 1.8f + 32;
  }
  Humidity = Sensor->readHumidity();
}

float DHTSensor::getTemp()
{
  return Temp;
}

char *DHTSensor::getTempText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_temp(Temp);
  else
    return toText(Temp);
}

float DHTSensor::getHumidity()
{
  return Humidity;
}

char *DHTSensor::getHumidityText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_percentage(Humidity);
  else
    return toText(Humidity);
}