#include "TDSSensor.h"

TDSSensor::TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings) : Common(Name), Pin(DefaultSettings->Pin), PowerPin(DefaultSettings->PowerPin), Parent(Parent)
{
  pinMode(Pin, INPUT);
  pinMode(PowerPin, OUTPUT);
  digitalWrite(PowerPin, HIGH); // Turn on power
  delay(50);
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("TDSSensor ready"), true, 3);
  digitalWrite(PowerPin, LOW); // Turn off power
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

void TDSSensor::refresh_Minute()
{
  Common::refresh_Minute();
  updateTDS(false);
}

void TDSSensor::updateTDS(bool ShowRaw)
{
  digitalWrite(PowerPin, HIGH); // Turn on power
  delay(50);
  int TDSRaw = analogRead(Pin);
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
    if (Metric)
    {
      Voltage = Voltage / (1.0 + 0.02 * (WaterTempSensor1->getTemp() - 25.0)); // Compensate TDS reading with temperature
    }
    else
    {
      Voltage = Voltage / (1.0 + 0.02 * ((WaterTempSensor1->getTemp() - 32) * 0.55555 - 25.0)); // Compensate TDS reading with temperature
    }
  }
  TDS = (int)((133.42 * Voltage * Voltage * Voltage - 255.86 * Voltage * Voltage + 857.39 * Voltage) * 50 + 0.5) / 100.0;
  if (TDS > 9999.99)
    TDS = 9999.99; // Maximum limit for positive values
  else if (TDS < -9999.99)
    TDS = -9999.99; // Maximum limit for negative values
  digitalWrite(PowerPin, LOW); // Turn off power
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