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
#include "src/Components/WaterPump.h"


HempyModule::HempyModule(const __FlashStringHelper *Name, Settings::HempyModuleSettings *DefaultSettings) : Common(Name), Module()
{ 
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (byte *)(((byte *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this -> SoundFeedback = Sound1;
  //DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  //PHSensor1 = new PHSensor(F("PHS1"), this, &ModuleSettings->PHSensor1);
  //WaterTemp1 = new WaterTempSensor(F("WaterT1"), this, &ModuleSettings->WaterTemp1);
  //WaterLevel1 = new WaterLevelSensor(F("WaterLevel1"), this, &ModuleSettings->WaterLevel1);
  Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);
  Weight2 = new WeightSensor(F("Weight2"), this, &ModuleSettings->Weight2);
  Pump1 = new WaterPump(F("Pump1"), this, &ModuleSettings->Pump1);
  Pump2 = new WaterPump(F("Pump2"), this, &ModuleSettings->Pump2);
  Bucket1 = new HempyBucket(F("Bucket1"), this, &ModuleSettings->Bucket1,Weight1,Pump1);
  Bucket2 = new HempyBucket(F("Bucket2"), this, &ModuleSettings->Bucket2,Weight2,Pump2);
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
  if(Command -> DisablePump1) Pump1 -> disable();
  if(Command -> TurnOnPump1) Pump1 -> turnOn(true);
  if(Command -> TurnOffPump1) Pump1 -> turnOff(true);
  *(Pump1 -> Timeout) = Command -> TimeOutPump1;
  Bucket1 -> setStartWeight(Command -> StartWeightBucket1);
  Bucket1 -> setStopWeight(Command -> StopWeightBucket1);

  if(Command -> DisablePump2) Pump2 -> disable();
  if(Command -> TurnOnPump2) Pump2 -> turnOn(true);
  if(Command -> TurnOffPump2) Pump2 -> turnOff(true);
  *(Pump2 -> Timeout) = Command -> TimeOutPump2;
  Bucket2 -> setStartWeight(Command -> StartWeightBucket2);
  Bucket2 -> setStopWeight(Command -> StopWeightBucket2);

  Serial.print(Command -> DisablePump1);
        Serial.print(", ");
        Serial.print(Command -> TurnOnPump1);
        Serial.print(", ");
        Serial.print(Command -> TurnOffPump1);
        Serial.print(", ");
        Serial.print(Command -> TimeOutPump1);
        Serial.print(", ");
        Serial.print(Command -> StartWeightBucket1);
        Serial.print(", ");
        Serial.print(Command -> StopWeightBucket1);
        Serial.print(" ; ");
  Serial.print(Command -> DisablePump2);
        Serial.print(", ");
        Serial.print(Command -> TurnOnPump2);
        Serial.print(", ");
        Serial.print(Command -> TurnOffPump2);
        Serial.print(", ");
        Serial.print(Command -> TimeOutPump2);
        Serial.print(", ");
        Serial.print(Command -> StartWeightBucket2);
        Serial.print(", ");
        Serial.println(Command -> StopWeightBucket2);
        

  updateResponse();       
}

void HempyModule::updateResponse(){
  Response.Bucket1Weight = Weight1 -> getWeight();
  Response.Bucket2Weight = Weight2 -> getWeight();
  Response.Temp = DHT1 -> getTemp();
  Response.Humidity = DHT1 -> getHumidity();

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
