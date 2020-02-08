#include "WaterLevelSensor.h"
#include "GrowBox.h"

WaterLevelSensor::WaterLevelSensor(const __FlashStringHelper *Name, GrowBox *GBox, Settings::WaterLevelSensorSettings *DefaultSettings) : Common(Name)
{ //constructor
  this->GBox = GBox;
  Pin_1 = &DefaultSettings->Pin_1;
  Pin_2 = &DefaultSettings->Pin_2;
  Pin_3 = &DefaultSettings->Pin_3;
  Pin_4 = &DefaultSettings->Pin_4;
  pinMode(*Pin_1, INPUT_PULLUP);
  pinMode(*Pin_2, INPUT_PULLUP);
  pinMode(*Pin_3, INPUT_PULLUP);
  pinMode(*Pin_4, INPUT_PULLUP);
  GBox->AddToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  GBox->AddToRefreshQueue_Minute(this);  //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  GBox->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  logToSerials(F("WaterLevelSensor object created"), true, 1);
}

void WaterLevelSensor::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Level")), getLevelGauge());
  }
}

void WaterLevelSensor::refresh_Minute()
{
  if (*DebugEnabled)
    Common::refresh_Minute();
  bool isAboveSensor1 = !digitalRead(*Pin_1); //Empty: Lowest Water sensor, true if level reached
  bool isAboveSensor2 = !digitalRead(*Pin_2);
  bool isAboveSensor3 = !digitalRead(*Pin_3);
  bool isAboveSensor4 = !digitalRead(*Pin_4); //Full: Highest Water sensor, true if level reached

  memset(&LevelGauge, 0, sizeof(LevelGauge)); //clear variable
  strcpy_P(LevelGauge, (PGM_P)F("E["));
  if (isAboveSensor1)
    strcat_P(LevelGauge, (PGM_P)F("#"));
  else
    strcat_P(LevelGauge, (PGM_P)F("_")); //Empty
  if (isAboveSensor2)
    strcat_P(LevelGauge, (PGM_P)F("#"));
  else
    strcat_P(LevelGauge, (PGM_P)F("_"));
  if (isAboveSensor3)
    strcat_P(LevelGauge, (PGM_P)F("#"));
  else
    strcat_P(LevelGauge, (PGM_P)F("_"));
  if (isAboveSensor4)
    strcat_P(LevelGauge, (PGM_P)F("#"));
  else
    strcat_P(LevelGauge, (PGM_P)F("_")); //Full
  strcat_P(LevelGauge, (PGM_P)F("]F"));

  //Get number representation of reservoir level 0-empty to 4-full
  if (!isAboveSensor1 && !isAboveSensor2 && !isAboveSensor3 && !isAboveSensor4)
    Level = 0; //Empty
  else if (isAboveSensor1 && !isAboveSensor2 && !isAboveSensor3 && !isAboveSensor4)
    Level = 1;
  else if (isAboveSensor1 && isAboveSensor2 && !isAboveSensor3 && !isAboveSensor4)
    Level = 2;
  else if (isAboveSensor1 && isAboveSensor2 && isAboveSensor3 && !isAboveSensor4)
    Level = 3;
  else if (isAboveSensor1 && isAboveSensor2 && isAboveSensor3 && isAboveSensor4)
    Level = 4; //Full
  else
    Level = -1; //non-valid sensor combination, like: E[#--#]F
}

void WaterLevelSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("Level:"));
  strcat(LongMessage, getLevelGauge());
  strcat_P(LongMessage, (PGM_P)F(" ("));
  strcat(LongMessage, getLevelText());
  strcat_P(LongMessage, (PGM_P)F(")"));
  logToSerials(&LongMessage, true, 1);
}

int8_t WaterLevelSensor::getLevel()
{
  return Level;
}

char *WaterLevelSensor::getLevelText()
{
  return toText(Level);
}

char *WaterLevelSensor::getLevelGauge()
{
  return LevelGauge;
}