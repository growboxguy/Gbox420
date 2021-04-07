#include "TDSSensor.h"

TDSSensor::TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings) : Common(Name)
{ ///< constructor
  this->Parent = Parent;
  this->Pin = &DefaultSettings->Pin;
  this->PowerPin = &DefaultSettings->PowerPin;
  pinMode(*Pin, INPUT);
  pinMode(*PowerPin, OUTPUT);
  digitalWrite(*PowerPin, HIGH); //Turn on power
  delay(50);
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("TDSSensor ready"), true, 3);
  digitalWrite(*PowerPin, LOW); //Turn off power
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void TDSSensor::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"T\":\""));
  strcat(LongMessage, getTDSText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

TDSSensor::TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings, WaterTempSensor *WaterTempSensor1) : TDSSensor(Name, Parent, DefaultSettings)
{ ///< constructor with an extra WaterTemperature object
  this->WaterTempSensor1 = WaterTempSensor1;
}

void TDSSensor::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  updateTDS(false);
}

void TDSSensor::updateTDS(bool ShowRaw)
{
  digitalWrite(*PowerPin, HIGH); //Turn on power
  delay(50);
  int TDSRaw = analogRead(*Pin);
  if (ShowRaw)
  {
    appendName(true);
    strcat_P(ShortMessage, (PGM_P)F("TDS reading: "));
    strcat(ShortMessage, toText(TDSRaw));
    Parent->addToLog(ShortMessage);
  }

  float Voltage = ((float)TDSRaw) * 5 / 1024;
  if (WaterTempSensor1) // if a Water Temperature is available
  {
    if (*Metric)
    {
      Voltage = Voltage / (1.0 + 0.02 * (WaterTempSensor1->getTemp() - 25.0)); //Compensate TDS reading with temperature
    }
    else
    {
      Voltage = Voltage / (1.0 + 0.02 * ((WaterTempSensor1->getTemp() - 32) * 0.55555 - 25.0)); //Compensate TDS reading with temperature
    }
  }
  TDS = (float)((133.42 * pow(Voltage, 3) - 255.86 * pow(Voltage, 2) + 857.39 * Voltage) * 0.5);
  digitalWrite(*PowerPin, LOW); //Turn off power
}

float TDSSensor::getTDS()
{
  return TDS;
}

char *TDSSensor::getTDSText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_TDS(getTDS());
  else
    return toText(getTDS());
}