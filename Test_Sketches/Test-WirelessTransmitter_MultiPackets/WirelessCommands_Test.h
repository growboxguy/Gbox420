#pragma once

#include "TimeLib.h"     ///< keeping track of time
///Structs for wireless communication

///Global constants
enum HempyMessage { Module1Command,Module1Response,Bucket1Command,Bucket1Response,Bucket2Command,Bucket2Response,DHT1Response,GetNext};  ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824
//enum MessageType { Command, Response };

const __FlashStringHelper* sequenceIDToText(enum HempyMessage SequenceID) 
{
   switch (SequenceID) 
   {
      case HempyMessage::Module1Command: return F("Module1Command"); break;
      case HempyMessage::Module1Response: return F("Module1Response"); break;
      case HempyMessage::Bucket1Command: return F("Bucket1Command"); break;
      case HempyMessage::Bucket1Response: return F("Bucket1Response"); break;
      case HempyMessage::Bucket2Command: return F("Bucket2Command"); break;
      case HempyMessage::Bucket2Response: return F("Bucket2Response"); break;
      case HempyMessage::DHT1Response: return F("DHT1Response"); break;
      case HempyMessage::GetNext: return F("GetNext"); break;
      default : return F("UNKNOWN"); break;
   }
}
                                                                                                                                         ///< GetNext should always be the last element
struct commonTemplate  ///< Shared between Command and Respone packages
{
   uint8_t SequenceID;  ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data
   
   commonTemplate(uint8_t SequenceID){
      this -> SequenceID = SequenceID;
   }
};

///< Both the Transmitter and the Receiver needs to know these structures

///< Commands are sent by the Main module
struct ModuleCommand : commonTemplate  //Max 32bytes. Module command sent by the Main module
{
    ModuleCommand(HempyMessage SequenceID) : commonTemplate(SequenceID){}
    ModuleCommand(HempyMessage SequenceID,time_t Time, bool Debug, bool Metric) : commonTemplate(SequenceID){}
    time_t Time = 0; 
    bool Debug = true;
    bool Metric = true;
};

struct BucketCommand : commonTemplate  //Max 32bytes. Command to control one hempy bucket
{
    BucketCommand(HempyMessage SequenceID) : commonTemplate(SequenceID){}
    BucketCommand(HempyMessage SequenceID, bool DisablePump, bool TurnOnPump, bool TurnOffPump, int TimeOutPump, float StartWeight, float StopWeight) : commonTemplate(SequenceID){}   
    bool DisablePump = false;
    bool TurnOnPump = false;
    bool TurnOffPump = false;
    int TimeOutPump = 0;
    float StartWeight = 0.0;
    float StopWeight = 0.0;
};

///< Responses are sent by all other modules, except Main  

struct ModuleResponse  : commonTemplate  //Max 32bytes. Module response sent back to the Main module
{
   ModuleResponse(HempyMessage SequenceID) : commonTemplate(SequenceID){}
   ModuleResponse(HempyMessage SequenceID, bool Status) : commonTemplate(SequenceID){}
   bool Status = true;   
};

struct BucketResponse  : commonTemplate  //Max 32bytes. Response from one hempy bucket
{
   BucketResponse(HempyMessage SequenceID) : commonTemplate(SequenceID){}
   BucketResponse(HempyMessage SequenceID, bool PumpOn, bool PumpEnabled, float Weight) : commonTemplate(SequenceID){}
   bool PumpOn = false; 
   bool PumpEnabled = false;
   float Weight = 0.0;
};

struct DHTResponse  : commonTemplate  //Max 32bytes. Response from a DHT sensor
{
   DHTResponse(HempyMessage SequenceID) : commonTemplate(SequenceID){}
   DHTResponse(HempyMessage SequenceID, float Temp, float Humidity) : commonTemplate(SequenceID){}
   float Temp = 0.0;
   float Humidity = 0.0;
};