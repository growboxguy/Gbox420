#include "TDSSensor.h"

TDSSensor::TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings) : Common(Name)
{ ///< constructor
  this->Parent = Parent;
  this->Pin = &DefaultSettings->Pin;
  pinMode(*Pin, INPUT);
  AverageTDS = new movingAvg(MovingAverageDepth);
  AverageTDS->begin();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("TDSSensor ready"), true, 3);
}

void TDSSensor::report(bool JSONReport)
{
  Common::report(JSONReport); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    strcat_P(LongMessage, (PGM_P)F("\"T\":\""));
    strcat(LongMessage, getTDSText(false, true));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else //Print a report to the Serial console
  {
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("TDS:"));
    strcat(LongMessage, getTDSText(false, true));
    strcat_P(LongMessage, (PGM_P)F(" ; Average:"));
    strcat(LongMessage, getTDSText(true, true));
    logToSerials(&LongMessage, true, 1);
  }
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

void TDSSensor::updateTDS(bool ShowRaw)
{
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
  AverageTDS->reading(TDS * 10); //Reading will be parsed to integer, keep first decimal digit
}

float TDSSensor::getTDS(bool ReturnAverage)
{
  if (ReturnAverage)
    return AverageTDS->getAvg() / 10.0;
  else
    return TDS;
}

char *TDSSensor::getTDSText(bool ReturnAverage, bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_TDS(getTDS(ReturnAverage));
  else
    return toText(getTDS(ReturnAverage));
}