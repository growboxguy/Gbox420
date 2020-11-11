/**@file*/
///Supports monitoring two Hempy buckets
///Runs autonomously on an Arduino Nano RF and communicates wirelessly with the main module

#include "HempyModule.h"
//#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/WeightSensor.h"
#include "../Components/WaterPump.h"
#include "../Components/HempyBucket.h"

///< Variables used during wireless communication
uint8_t NextSequenceID = HempyMessages::HempyModuleResponse1;
struct HempyModuleResponse HempyResponse1ToSend = {HempyMessages::HempyModuleResponse1};
struct HempyBucketResponse HempyBucket1ResponseToSend = {HempyMessages::HempyBucketResponse1};
struct HempyBucketResponse HempyBucket2ResponseToSend = {HempyMessages::HempyBucketResponse2};
struct HempyCommonTemplate HempyLastResponseToSend = {HempyMessages::HempyGetNext};  //< Special response signaling the end of a message exchange to the Transmitter

HempyModule::HempyModule(const __FlashStringHelper *Name, Settings::HempyModuleSettings *DefaultSettings) : Common(Name), Module()
{ 
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this -> SoundFeedback = Sound1;
  WeightB1 = new WeightSensor(F("WeightB1"), this, &ModuleSettings->WeightB1);
  WeightB2 = new WeightSensor(F("WeightB2"), this, &ModuleSettings->WeightB2);
  WeightWR1 = new WeightSensor(F("WeightWR1"), this, &ModuleSettings->WeightWR1);
  WeightWR2 = new WeightSensor(F("WeightWR2"), this, &ModuleSettings->WeightWR2);
  Pump1 = new WaterPump(F("Pump1"), this, &ModuleSettings->HempyPump1);
  Pump2 = new WaterPump(F("Pump2"), this, &ModuleSettings->HempyPump2);
  Bucket1 = new HempyBucket(F("Bucket1"), this, &ModuleSettings->Bucket1,WeightB1,Pump1);
  Bucket2 = new HempyBucket(F("Bucket2"), this, &ModuleSettings->Bucket2,WeightB2,Pump2);
  addToRefreshQueue_Sec(this);         
  addToRefreshQueue_FiveSec(this);     
  //addToRefreshQueue_Minute(this);    
  //addToRefreshQueue_QuarterHour(this); 
  logToSerials(Name, false, 0);
  logToSerials(F("- HempyModule object created, refreshing..."), true, 1);
  runAll();
  addToLog(F("HempyModule initialized"), 0);
}

void HempyModule::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  if(NextSequenceID != HempyMessages::HempyModuleResponse1 && millis()- LastMessageReceived >= WirelessMessageTimeout){  //< If there is a package exchange in progress, but a followup command was not received within the timeout
      NextSequenceID = HempyMessages::HempyModuleResponse1;  //< Reset back to the first response
      logToSerials(F("Timeout during message exchange, reseting to first response"),true,0);   
      updateAckData();  
  } 
}

void HempyModule::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec(); 
  runReport(); 
  updateResponse(); 
}

void HempyModule::updateResponse(){
  HempyBucket1ResponseToSend.PumpState = Pump1 -> getState();;
  HempyBucket1ResponseToSend.Weight = WeightB1 -> getWeight();
  HempyBucket2ResponseToSend.PumpState = Pump2 -> getState();
  HempyBucket2ResponseToSend.Weight = WeightB2 -> getWeight(); 
  updateAckData();
}

void HempyModule::processCommand(void *ReceivedCommand){
  HempyMessages ReceivedSequenceID = ((HempyCommonTemplate*)ReceivedCommand) -> SequenceID;
  LastMessageReceived = millis();  ///< Store current time
  if(*Debug){
      logToSerials(F("Received SequenceID:"),false,1);
      logToSerials(ReceivedSequenceID,false,1);
      logToSerials(F("-"),false,1);
      logToSerials(toText_hempySequenceID(ReceivedSequenceID),false,1);
      logToSerials(F(", Acknowledgement sent with SequenceID:"),false,0);
      logToSerials(NextSequenceID,false,1);
      logToSerials(F("-"),false,1);
      logToSerials(toText_hempySequenceID(NextSequenceID),true,1);
  } 

  switch (ReceivedSequenceID){
    case HempyMessages::HempyModuleCommand1 :
      setDebug(((HempyModuleCommand*)ReceivedCommand) -> Debug);
      setMetric(((HempyModuleCommand*)ReceivedCommand) -> Metric);
      NextSequenceID = HempyMessages::HempyBucketResponse1;  // update the next Message that will be copied to the buffer 
      if(*Debug){
        logToSerials(F("Module:"),false,2);
        logToSerials(((HempyModuleCommand*)ReceivedCommand) -> Time,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyModuleCommand*)ReceivedCommand) -> Debug,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyModuleCommand*)ReceivedCommand) -> Metric,true,1);
      }            
      break;
    case HempyMessages::HempyBucketCommand1 :
      if(((HempyBucketCommand*)ReceivedCommand) -> DisablePump) Pump1 -> disablePump();
      if(((HempyBucketCommand*)ReceivedCommand) -> TurnOnPump) Bucket1 -> startWatering();
      if(((HempyBucketCommand*)ReceivedCommand) -> TurnOffPump) Pump1 -> stopPump();
      Pump1 -> setPumpTimeOut(((HempyBucketCommand*)ReceivedCommand) -> TimeOutPump);
      Pump1 -> setSpeed(((HempyBucketCommand*)ReceivedCommand) -> PumpSpeed);
      Bucket1 -> setWeightBasedWatering(((HempyBucketCommand*)ReceivedCommand) -> WeightBasedWatering);
      Bucket1 -> setStartWeight(((HempyBucketCommand*)ReceivedCommand) -> StartWeight);
      Bucket1 -> setStopWeight(((HempyBucketCommand*)ReceivedCommand) -> StopWeight);
      Bucket1 -> setTimerBasedWatering(((HempyBucketCommand*)ReceivedCommand) -> TimerBasedWatering);
      Bucket1 -> setWateringInterval(((HempyBucketCommand*)ReceivedCommand) -> WateringInterval);
      Bucket1 -> setWateringDuration(((HempyBucketCommand*)ReceivedCommand) -> WateringDuration);
      NextSequenceID = HempyMessages::HempyBucketResponse2;  // update the next Message that will be copied to the buffer
      if(*Debug){  
        logToSerials(F("Bucket1:"),false,2);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> DisablePump,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> TurnOnPump,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> TurnOffPump,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> PumpSpeed,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> TimeOutPump,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> WeightBasedWatering,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> StartWeight,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> StopWeight,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> TimerBasedWatering,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> WateringInterval,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> WateringDuration,true,1);
      }      
      break;
    case HempyMessages::HempyBucketCommand2 :
      if(((HempyBucketCommand*)ReceivedCommand) -> DisablePump) Pump2 -> disablePump();
      if(((HempyBucketCommand*)ReceivedCommand) -> TurnOnPump) Bucket2 -> startWatering();
      if(((HempyBucketCommand*)ReceivedCommand) -> TurnOffPump) Pump2 -> stopPump();
      Pump2 -> setPumpTimeOut(((HempyBucketCommand*)ReceivedCommand) -> TimeOutPump);
      Pump2 -> setSpeed(((HempyBucketCommand*)ReceivedCommand) -> PumpSpeed);
      Bucket2 -> setWeightBasedWatering(((HempyBucketCommand*)ReceivedCommand) -> WeightBasedWatering);
      Bucket2 -> setStartWeight(((HempyBucketCommand*)ReceivedCommand) -> StartWeight);
      Bucket2 -> setStopWeight(((HempyBucketCommand*)ReceivedCommand) -> StopWeight);
      Bucket2 -> setTimerBasedWatering(((HempyBucketCommand*)ReceivedCommand) -> TimerBasedWatering);
      Bucket2 -> setWateringInterval(((HempyBucketCommand*)ReceivedCommand) -> WateringInterval);
      Bucket2 -> setWateringDuration(((HempyBucketCommand*)ReceivedCommand) -> WateringDuration);
      NextSequenceID = HempyMessages::HempyGetNext; // update the next Message that will be copied to the buffer 
      if(*Debug){
        logToSerials(F("Bucket2:"),false,2);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> DisablePump,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> TurnOnPump,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> TurnOffPump,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> PumpSpeed,false,1);        
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> TimeOutPump,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> WeightBasedWatering,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> StartWeight,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> StopWeight,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> TimerBasedWatering,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> WateringInterval,false,1);
        logToSerials(F(","),false,1);
        logToSerials(((HempyBucketCommand*)ReceivedCommand) -> WateringDuration,true,1);
      }                
      break;
    case HempyMessages::HempyGetNext :     //< Used to get all Responses that do not have a corresponding Command 
      if(++NextSequenceID > HempyMessages::HempyGetNext){  //< If the end of HempyMessages enum is reached
          NextSequenceID = HempyMessages::HempyModuleResponse1; //< Load the first response for the next message exchange
          if(Debug){ logToSerials(F("Message exchange finished"),true,0);  }
      }            
      break;
    default:
      logToSerials(F("SequenceID unknown, ignoring message"),true,2);
      break;        
  } 
  updateAckData();   //< Loads the next ACK that will be sent out
  saveSettings(ModuleSettings); //< Store changes in EEPROM  
}

void HempyModule::updateAckData() { // so you can see that new data is being sent

    logToSerials(F("Updating Acknowledgement message to responseID:"),false,2);
    logToSerials(NextSequenceID,true,1);
    Wireless.flush_tx();  ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved) 

    switch (NextSequenceID)  // based on the NextSeqenceID load the next response into the Acknowledgement buffer
    {        
    case HempyMessages::HempyModuleResponse1 :
        Wireless.writeAckPayload(1, &HempyResponse1ToSend, WirelessPayloadSize);  
        break;
    case HempyMessages::HempyBucketResponse1 :
        Wireless.writeAckPayload(1, &HempyBucket1ResponseToSend, WirelessPayloadSize);
        break;   
    case HempyMessages::HempyBucketResponse2 :
        Wireless.writeAckPayload(1, &HempyBucket2ResponseToSend, WirelessPayloadSize);
        break;   
    case HempyMessages::HempyGetNext :  //< HempyGetNext should always be the last element in the enum: Signals to stop the message exchange
        Wireless.writeAckPayload(1, &HempyLastResponseToSend, WirelessPayloadSize);
        break;
    default:
        logToSerials(F("Unknown next Sequence number, Ack defaults loaded"),true,3); 
        Wireless.writeAckPayload(1, &HempyResponse1ToSend, WirelessPayloadSize); // load the first Response into the buffer 
        break;    
    }
}