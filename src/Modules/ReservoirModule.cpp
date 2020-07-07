#include "ReservoirModule.h"
#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/PHSensor.h"
#include "../Components/WaterTempSensor.h"
#include "../Components/WeightSensor.h"

ReservoirModule::ReservoirModule(const __FlashStringHelper *Name, Settings::ReservoirModuleSettings *DefaultSettings) : Common(Name), Module()
{ 
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this -> SoundFeedback = Sound1;
  DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  WTemp1 = new WaterTempSensor(F("WTemp1"), this, &ModuleSettings->WTemp1);
  PHSen1 = new PHSensor(F("PHSen1"), this, &ModuleSettings->PHSen1); 
  Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);  
  //addToRefreshQueue_Sec(this);         
  addToRefreshQueue_FiveSec(this);     
  //addToRefreshQueue_Minute(this);    
  //addToRefreshQueue_QuarterHour(this); 
  logToSerials(Name, false, 0);
  logToSerials(F("- ReservoirModule object created, refreshing..."), true, 1);
  runAll();
  addToLog(F("ReservoirModule initialized"), 0);
}

void ReservoirModule::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec(); 
  runReport(); 
  updateResponse(); 
}

void ReservoirModule::updateResponse(){  ///<Updates the response sent to the Main module
  Response.PH = PHSen1 -> getPH();
  Response.Weight = Weight1 -> getWeight();
  Response.WaterTemperature = WTemp1 -> getTemp();
  Response.AirTemperature = DHT1 -> getTemp();
  Response.Humidity = DHT1 -> getHumidity();
  Wireless.flush_tx();  ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved) 
  Wireless.writeAckPayload(1, &Response, sizeof(Response)); ///< load the payload to send the next time a command is received
}

void ReservoirModule::processCommand(reservoirCommand *Command){
  setDebug(Command -> Debug);
  setMetric(Command -> Metric);

  updateResponse();
  saveSettings(ModuleSettings);

  if(*Debug){
    logToSerials(Command -> Debug,false,3);
    logToSerials(F(","),false,1);
    logToSerials(Command -> Metric,false,1);
    logToSerials(F(";"),true,1); 
  }       
}