#include "PressureSensor.h"
#include "../GrowBox.h"

PressureSensor::PressureSensor(const __FlashStringHelper *Name, GrowBox *GBox, Settings::PressureSensorSettings *DefaultSettings) : Common(Name)
{
  this->GBox = GBox;
  Pin = &DefaultSettings->Pin;
  Ratio = &DefaultSettings->Ratio;
  Offset = &DefaultSettings->Offset;
  Pressure = new RollingAverage();
  GBox->AddToReportQueue(this);         //Subscribing to the report queue: Calls the report() method
  GBox->AddToRefreshQueue_Minute(this); //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  GBox->AddToWebsiteQueue_Load(this);   //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  GBox->AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  GBox->AddToWebsiteQueue_Field(this);  //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  logToSerials(F("Pressure Sensor object created"), true, 1);
}

void PressureSensor::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/Settings.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Offset")), toPrecisionText(*Offset));
    WebServer.setArgString(getWebsiteComponentName(F("Ratio")), toPrecisionText(*Ratio));
  }
}

void PressureSensor::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("ReadOffset")) == 0)
    {
      readOffset();
    }
  }
}

void PressureSensor::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Offset")) == 0)
    {
      setOffset(WebServer.getArgFloat());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Ratio")) == 0)
    {
      setRatio(WebServer.getArgFloat());
    }
  }
}

void PressureSensor::refresh_Minute()
{
  if (*DebugEnabled)
    Common::refresh_Minute();
  readPressure();
}

void PressureSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("Pressure:"));
  strcat(LongMessage, getPressureText(true, true));
  logToSerials(&LongMessage, true, 1);
}

void PressureSensor::readPressure()
{
  float Voltage = ((float)analogRead(*Pin)) * 5 / 1024;
  if (*MetricSystemEnabled)
    Pressure->updateAverage(*Ratio * (Voltage - *Offset) * 1.0f); // unit: bar / 100kPa
  else
    Pressure->updateAverage(*Ratio * (Voltage - *Offset) * 14.5038f); //unit: PSI
}

float PressureSensor::getPressure(bool ReturnAverage)
{
  return Pressure->getFloat(ReturnAverage);
}

char *PressureSensor::getPressureText(bool IncludeUnits, bool ReturnAverage)
{
  if (IncludeUnits)
    return pressureToText(Pressure->getFloat(ReturnAverage));
  else
    return Pressure->getFloatText(ReturnAverage);
}

void PressureSensor::readOffset()
{ //Should only be called when there is 0 pressure
  float sum = 0;
  for (byte i = 0; i < 50; i++)
  {
    sum += analogRead(*Pin);
    delay(10);
  }
  float AeroOffsetRecommendation = (sum / 50) * 5 / 1024; //Reads voltage at 0 pressure
  strncpy_P(LongMessage, (PGM_P)F("0 pressure Offset: "), MaxTextLength);
  strcat(LongMessage, toText(AeroOffsetRecommendation));
  GBox->addToLog(LongMessage);
}

void PressureSensor::setOffset(float Value)
{
  *Offset = Value;
  Pressure->resetAverage();
  GBox->addToLog(F("Pressure offset updated"));
}

void PressureSensor::setRatio(float Value)
{
  *Ratio = Value;
  Pressure->resetAverage();
  GBox->addToLog(F("Pressure/voltage ratio updated"));
}