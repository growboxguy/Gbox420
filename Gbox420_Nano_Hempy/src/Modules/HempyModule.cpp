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
uint8_t NextSequenceID = HempyMessages::Module1Response;
struct ModuleResponse Module1ResponseToSend = {HempyMessages::Module1Response};
struct BucketResponse Bucket1ResponseToSend = {HempyMessages::Bucket1Response};
struct BucketResponse Bucket2ResponseToSend = {HempyMessages::Bucket2Response};
struct CommonTemplate LastResponseToSend = {HempyMessages::GetNext};  //< Special response signaling the end of a message exchange to the Transmitter


HempyModule::HempyModule(const __FlashStringHelper *Name, Settings::HempyModuleSettings *DefaultSettings) : Common(Name), Module()
{ 
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this -> SoundFeedback = Sound1;
  Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);
  Weight2 = new WeightSensor(F("Weight2"), this, &ModuleSettings->Weight2);
  Pump1 = new WaterPump(F("Pump1"), this, &ModuleSettings->HempyPump1);
  Pump2 = new WaterPump(F("Pump2"), this, &ModuleSettings->HempyPump2);
  Bucket1 = new HempyBucket(F("Bucket1"), this, &ModuleSettings->Bucket1,Weight1,Pump1);
  Bucket2 = new HempyBucket(F("Bucket2"), this, &ModuleSettings->Bucket2,Weight2,Pump2);
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
  if(NextSequenceID != HempyMessages::Module1Response && millis()- LastMessageReceived >= WirelessMessageTimeout){  //< If there is a package exchange in progress
      NextSequenceID = HempyMessages::Module1Response;  //< Reset back to the first response
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
  Bucket1ResponseToSend.PumpState = Pump1 -> getState();;
  Bucket1ResponseToSend.Weight = Weight1 -> getWeight();
  Bucket2ResponseToSend.PumpState = Pump2 -> getState();
  Bucket2ResponseToSend.Weight = Weight2 -> getWeight(); 
  updateAckData();
}

void HempyModule::processCommand(void *ReceivedCommand){
  HempyMessages ReceivedSequenceID = ((CommonTemplate*)ReceivedCommand) -> SequenceID;
  LastMessageReceived = millis();  ///< Store current time
  if(*Debug){
      logToSerials(F("Command received with SequenceID: "),false,0);
      logToSerials(ReceivedSequenceID,false,0);
      logToSerials(F("- "),false,1);
      logToSerials(toText_hempySequenceID(ReceivedSequenceID),false,0);
      logToSerials(F(", Acknowledgement sent with SequenceID: "),false,0);
      logToSerials(NextSequenceID,false,0);
      logToSerials(F("- "),false,1);
      logToSerials(toText_hempySequenceID(NextSequenceID),true,0);
  } 

  switch (ReceivedSequenceID){
    case HempyMessages::Module1Command :
      setDebug(((ModuleCommand*)ReceivedCommand) -> Debug);
      setMetric(((ModuleCommand*)ReceivedCommand) -> Metric);
      NextSequenceID = HempyMessages::Bucket1Response;  // update the next Message that will be copied to the buffer 
      if(*Debug){
        logToSerials(F("Module: "),false,2);
        logToSerials(((ModuleCommand*)ReceivedCommand) -> Time,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((ModuleCommand*)ReceivedCommand) -> Debug,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((ModuleCommand*)ReceivedCommand) -> Metric,true,0);
      }            
      break;
    case HempyMessages::Bucket1Command :
      if(((BucketCommand*)ReceivedCommand) -> DisablePump) Pump1 -> disablePump();
      if(((BucketCommand*)ReceivedCommand) -> TurnOnPump) Pump1 -> startPump(true);
      if(((BucketCommand*)ReceivedCommand) -> TurnOffPump) Pump1 -> stopPump();
      Pump1 -> setPumpTimeOut(((BucketCommand*)ReceivedCommand) -> TimeOutPump);
      Bucket1 -> setStartWeight(((BucketCommand*)ReceivedCommand) -> StartWeight);
      Bucket1 -> setStopWeight(((BucketCommand*)ReceivedCommand) -> StopWeight);
      NextSequenceID = HempyMessages::Bucket2Response;  // update the next Message that will be copied to the buffer
      if(*Debug){  
        logToSerials(F("Bucket1: "),false,2);
        logToSerials(((BucketCommand*)ReceivedCommand) -> DisablePump,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((BucketCommand*)ReceivedCommand) -> TurnOnPump,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((BucketCommand*)ReceivedCommand) -> TurnOffPump,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((BucketCommand*)ReceivedCommand) -> TimeOutPump,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((BucketCommand*)ReceivedCommand) -> StartWeight,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((BucketCommand*)ReceivedCommand) -> StopWeight,true,0);
      }      
      break;
    case HempyMessages::Bucket2Command :
      if(((BucketCommand*)ReceivedCommand) -> DisablePump) Pump2 -> disablePump();
      if(((BucketCommand*)ReceivedCommand) -> TurnOnPump) Pump2 -> startPump(true);
      if(((BucketCommand*)ReceivedCommand) -> TurnOffPump) Pump2 -> stopPump();
      Pump2 -> setPumpTimeOut(((BucketCommand*)ReceivedCommand) -> TimeOutPump);
      Bucket2 -> setStartWeight(((BucketCommand*)ReceivedCommand) -> StartWeight);
      Bucket2 -> setStopWeight(((BucketCommand*)ReceivedCommand) -> StopWeight);
      NextSequenceID = HempyMessages::GetNext; // update the next Message that will be copied to the buffer 
      if(*Debug){
        logToSerials(F("Bucket2: "),false,2);
        logToSerials(((BucketCommand*)ReceivedCommand) -> DisablePump,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((BucketCommand*)ReceivedCommand) -> TurnOnPump,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((BucketCommand*)ReceivedCommand) -> TurnOffPump,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((BucketCommand*)ReceivedCommand) -> TimeOutPump,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((BucketCommand*)ReceivedCommand) -> StartWeight,false,0);
        logToSerials(F(", "),false,0);
        logToSerials(((BucketCommand*)ReceivedCommand) -> StopWeight,true,0);
      }                
      break;
    case HempyMessages::GetNext :     //< Used to get all Responses that do not have a corresponding Command 
      if(++NextSequenceID > HempyMessages::GetNext){  //< If the end of HempyMessages enum is reached
          NextSequenceID = HempyMessages::Module1Response; //< Load the first response for the next message exchange
          if(Debug){ logToSerials(F("Message exchange finished"),true,0);  }
      }            
      break;
    default:
      logToSerials(F("  SequenceID unknown, ignoring message"),true,0);
      break;        
  } 
  updateAckData();   //< Loads the next ACK that will be sent out
  saveSettings(ModuleSettings); //< Store changes in EEPROM  
}

void HempyModule::updateAckData() { // so you can see that new data is being sent

    logToSerials(F("Updating Acknowledgement message to responseID: "),false,2);
    logToSerials(NextSequenceID,true,0);
    Wireless.flush_tx();  ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved) 

    switch (NextSequenceID)  // based on the NextSeqenceID load the next response into the Acknowledgement buffer
    {        
    case HempyMessages::Module1Response :
        Wireless.writeAckPayload(1, &Module1ResponseToSend, WirelessPayloadSize);  
        break;
    case HempyMessages::Bucket1Response :
        Wireless.writeAckPayload(1, &Bucket1ResponseToSend, WirelessPayloadSize);
        break;   
    case HempyMessages::Bucket2Response :
        Wireless.writeAckPayload(1, &Bucket2ResponseToSend, WirelessPayloadSize);
        break;   
    case HempyMessages::GetNext :  //< GetNext should always be the last element in the enum: Signals to stop the message exchange
        Wireless.writeAckPayload(1, &LastResponseToSend, WirelessPayloadSize);
        break;
    default:
        logToSerials(F("Unknown next Sequence number, Ack defaults loaded"),true,3); 
        Wireless.writeAckPayload(1, &Module1ResponseToSend, WirelessPayloadSize); // load the first Response into the buffer 
        break;    
    }
}