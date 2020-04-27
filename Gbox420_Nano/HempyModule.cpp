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
#include "src/Components/HempyBucket.h"


HempyModule::HempyModule(const __FlashStringHelper *Name, Settings::HempyModuleSettings *DefaultSettings) : Common(Name), Module()
{ 
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (byte *)(((byte *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  //DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  //PHSensor1 = new PHSensor(F("PHS1"), this, &ModuleSettings->PHSensor1);
  //WaterTemp1 = new WaterTempSensor(F("WaterT1"), this, &ModuleSettings->WaterTemp1);
  //WaterLevel1 = new WaterLevelSensor(F("WaterLevel1"), this, &ModuleSettings->WaterLevel1);
  Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);
  Weight2 = new WeightSensor(F("Weight2"), this, &ModuleSettings->Weight2);
  Bucket1 = new HempyBucket(F("Bucket1"), this, &ModuleSettings->Bucket1,Weight1);
  Bucket2 = new HempyBucket(F("Bucket2"), this, &ModuleSettings->Bucket2,Weight2);
  addToRefreshQueue_Sec(this);         ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method
  addToRefreshQueue_FiveSec(this);     ///Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  addToRefreshQueue_Minute(this);      ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  addToRefreshQueue_QuarterHour(this); ///Subscribing to the 30 minutes refresh queue: Calls the refresh_QuarterHour() method
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

void HempyModule::processCommand(commandTemplate *Command){
        Serial.print(Command -> pump1Enabled);
        Serial.print(", ");
        Serial.print(Command -> pump2Enabled);
        Serial.print(", ");
        Serial.print(Command -> pump1Stop);
        Serial.print(", ");
        Serial.print(Command -> pump2Stop);
        Serial.print(", ");
        Serial.print(Command -> bucket1StartWatering);
        Serial.print(", ");
        Serial.print(Command -> bucket2StartWatering);
        Serial.print(", ");
        Serial.print(Command -> bucket1StartWeight);
        Serial.print(", ");
        Serial.print(Command -> bucket1StopWeight);
        Serial.print(", ");
        Serial.print(Command -> bucket2StartWeight);
        Serial.print(", ");
        Serial.println(Command -> bucket2StopWeight);
        Serial.println(); 

        updateResponse();       
}

void HempyModule::updateResponse(){
  Response.bucket1Weight = Weight1 -> getWeight();
  Response.bucket2Weight = Weight2 -> getWeight();
  Response.temp = DHT1 -> getTemp();
  Response.humidity = DHT1 -> getHumidity();
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
   // WaterTemp1->Temp->resetAverage();
    RefreshAllRequested = true;
  }
  if (*Metric)
    addToLog(F("Metric units"));
  else
    addToLog(F("Imperial units"));
}
