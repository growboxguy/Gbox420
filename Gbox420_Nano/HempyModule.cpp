/**@file*/
///Supports monitoring a Hempy buckets
///Runs autonomously on an Arduino Nano RF and [WILL] support wireless connection towards the main module

#include "HempyModule.h"
#include "src/Components/DHTSensor.h"
#include "src/Components/Sound.h"
#include "src/Components/WeightSensor.h"
#include "src/Components/PHSensor.h"
#include "src/Components/WaterTempSensor.h"
#include "src/Components/WaterLevelSensor.h"


HempyModule::HempyModule(const __FlashStringHelper *Name, Settings::HempyModuleSettings *DefaultSettings) : Common(Name), Module()
{ //Constructor
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); //Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (byte *)(((byte *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  PHSensor1 = new PHSensor(F("PHS1"), this, &ModuleSettings->PHSensor1);
  WaterTemp1 = new WaterTempSensor(F("WaterT1"), this, &ModuleSettings->WaterTemp1);
  WaterLevel1 = new WaterLevelSensor(F("WaterLevel1"), this, &ModuleSettings->WaterLevel1);
  //Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);
  //Weight2 = new WeightSensor(F("Weight2"), this, &ModuleSettings->Weight2);
  addToRefreshQueue_Sec(this);
  addToRefreshQueue_FiveSec(this);     //Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  addToRefreshQueue_Minute(this);      //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  addToRefreshQueue_QuarterHour(this); //Subscribing to the 30 minutes refresh queue: Calls the refresh_QuarterHour() method
  logToSerials(Name, false, 0);
  logToSerials(F("- HempyModule object created, refreshing..."), true, 1);
  runAll();
  addToLog(F("HempyModule initialized"), 0);
}

void HempyModule::refresh_Sec()
{
    if (*Debug)
    Common::refresh_Sec();
 /*  if(ArduinoSerial.available())
  {
    char temp = ArduinoSerial.read();
    if(temp == 'q')
     {
        Weight1 -> triggerTare(); //Reset the scale to 0
        ArduinoSerial.println("Scale1 Tare");
     }
     else if(temp == 'a')
     {
        Weight2 -> triggerTare(); //Reset the scale to 0
        ArduinoSerial.println("Scale2 Tare");
     }
    else if(temp == 'w')
       {
        Weight1 -> triggerCalibration(1); //Reset the scale to 0
        ArduinoSerial.println("Scale1 calibrating");
       }
     else if(temp == 's')
       {
        Weight1 -> triggerCalibration(1); //Reset the scale to 0
        ArduinoSerial.println("Scale2 calibrating");
       }
  } */
}

void HempyModule::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  if (RefreshAllRequested)
  {
    RefreshAllRequested = false;
    runAll();
  }  
  runReport();
}

void HempyModule::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();  
}

void HempyModule::refresh_QuarterHour()
{
  if (*Debug)
    Common::refresh_QuarterHour();
}

//////////////////////////////////////////////////////////////////
//Settings
void HempyModule::setDebugOnOff(bool State)
{
  *Debug = State;
  if (*Debug)
  {
    addToLog(F("Debug enabled"));
    Sound1->playOnSound();
  }
  else
  {
    addToLog(F("Debug disabled"));
    Sound1->playOffSound();
  }
}

void HempyModule::setMetric(bool MetricEnabled)
{
  if (MetricEnabled != *Metric)
  { //if there was a change
    *Metric = MetricEnabled;
    //ModuleSettings -> IFanSwitchTemp = convertBetweenTempUnits(ModuleSettings -> IFanSwitchTemp);
    //Pres1->Pressure->resetAverage();
    DHT1->Temp->resetAverage();
   // WaterTemp1->Temp->resetAverage();
    RefreshAllRequested = true;
  }
  if (*Metric)
    addToLog(F("Metric units"));
  else
    addToLog(F("Imperial units"));
}
