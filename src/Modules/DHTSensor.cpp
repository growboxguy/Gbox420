#include "DHTSensor.h"
#include "../GrowBox.h"

DHTSensor::DHTSensor(const __FlashStringHelper *Name, GrowBox *GBox, Settings::DHTSensorSettings *DefaultSettings) : Common(Name)
{
  this->GBox = GBox;
  Sensor = new DHT(*(&DefaultSettings->Pin), *(&DefaultSettings->Type));
  Sensor->begin(); //dereference the pointer to the object and then call begin() on it. Same as (*Sensor).begin();
  Temp = new RollingAverage();
  Humidity = new RollingAverage();
  GBox->AddToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  GBox->AddToRefreshQueue_Minute(this);  //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  GBox->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  logToSerials(F("DHT Sensor object created"), true, 1);
}

void DHTSensor::websiteEvent_Refresh(__attribute__((unused)) char *url)
{ //When the website is refreshing
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Temp")), getTempText(true, false)); //Shows the latest reading
    WebServer.setArgString(getWebsiteComponentName(F("Humidity")), getHumidityText(true, false));
  }
}

void DHTSensor::refresh_Minute()
{
  if (*DebugEnabled)
    Common::refresh_Minute();
  if (*MetricSystemEnabled)
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
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("Temp:"));
  strcat(LongMessage, getTempText(true, true)); //Shows the average reading
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