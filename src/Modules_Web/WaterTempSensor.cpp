#include "WaterTempSensor.h"
#include "GrowBox.h"

WaterTempSensor::WaterTempSensor(const __FlashStringHelper *Name, GrowBox *GBox, Settings::WaterTempSensorSettings *DefaultSettings) : Common_Web(Name)
{ //constructor
  this->GBox = GBox;
  //pinMode(*Pin, INPUT);
  Temp = new RollingAverage();
  TempSensorWire = new OneWire(*(&DefaultSettings->Pin)); //Reservoir waterproof temperature sensor (DS18B20)
  TempSensor = new DallasTemperature(TempSensorWire);     //Reservoir waterproof temperature sensor (DS18B20)
  TempSensor->begin();
  GBox->AddToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  GBox->AddToRefreshQueue_Minute(this);  //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  GBox->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  logToSerials(F("WaterTempSensor object created"), true, 1);
}

void WaterTempSensor::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Temp")), getTempText(true, false));
  }
}

void WaterTempSensor::refresh_Minute()
{
  if (*DebugEnabled)
    Common_Web::refresh_Minute();
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
  Common_Web::report();
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