#include "TDSSensor.h"

TDSSensor::TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings) : Common(Name)
{ ///< constructor
  this->Parent = Parent;
  this->Pin = &DefaultSettings->Pin;
  pinMode(*Pin, INPUT);
  TDS = new RollingAverage();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("TDSSensor object created"), true, 1);
}

TDSSensor::TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings, WaterTempSensor *WaterTempSensor1) : TDSSensor(Name, Parent, DefaultSettings)
{ ///< constructor with an extra WaterTemperature object
  this->WaterTempSensor1 = WaterTempSensor1;
}

void TDSSensor::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  updateTDS(false);
}

void TDSSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("TDS:"));
  strcat(LongMessage, getTDSText());
  logToSerials(&LongMessage, true, 1);
}

void TDSSensor::updateTDS(bool ShowRaw)
{
  int TDSRaw = analogRead(*Pin);
  if (ShowRaw)
  {
    strncpy_P(LongMessage, (PGM_P)F("TDS analog read: "), MaxWordLength);
    strcat(LongMessage, toText(TDSRaw));
    Parent->addToLog(LongMessage);
  }

  float Voltage = ((float)TDSRaw) * 5 / 1024);
  if (*WaterTempSensor1 != NULL) // if a Water Temperature is available
  {
    Voltage = (Voltage / (1.0 + 0.02 * (WaterTempSensor1 -> getTemp() - 25.0));  //Compensate TDS reading with temperature
  }
  TDS->updateAverage((133.42 * pow(Voltage, 3) - 255.86 * pow(Voltage, 2) + 857.39 * Voltage) * 0.5);
}

float TDSSensor::getTDS(bool ReturnAverage)
{
  return TDS->getFloat(ReturnAverage);
}

char *TDSSensor::getTDSText(bool ReturnAverage)
{
  return toText_TDS(TDS->getFloatText(ReturnAverage));
}