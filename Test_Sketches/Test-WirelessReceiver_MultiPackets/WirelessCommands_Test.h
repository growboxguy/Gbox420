#pragma once

#include "TimeLib.h" ///keeping track of time
///Structs for wireless communication

///Global constants
enum HempyMessage
{
   Module1Command,
   Module1Response,
   Bucket1Command,
   Bucket1Response,
   Bucket2Command,
   Bucket2Response,
   DHT1Response,
   Reset
}; ///An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824
//enum MessageType { Command, Response };

const __FlashStringHelper *sequenceIDToText(uint8_t SequenceID)
{
   switch (SequenceID)
   {
   case HempyMessage::Module1Command:
      return F("Module1Command");
      break;
   case HempyMessage::Module1Response:
      return F("Module1Response");
      break;
   case HempyMessage::Bucket1Command:
      return F("Bucket1Command");
      break;
   case HempyMessage::Bucket1Response:
      return F("Bucket1Response");
      break;
   case HempyMessage::Bucket2Command:
      return F("Bucket2Command");
      break;
   case HempyMessage::Bucket2Response:
      return F("Bucket2Response");
      break;
   case HempyMessage::DHT1Response:
      return F("DHT1Response");
      break;
   case HempyMessage::Reset:
      return F("Reset");
      break;
   default:
      return F("UNKNOWN");
      break;
   }
}
///Reset should always be the last element
struct CommonTemplate ///Shared between Command and Respone packages
{
   HempyMessage SequenceID; ///Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data

   CommonTemplate(HempyMessage SequenceID)
   {
      this->SequenceID = SequenceID;
   }
};

///Both the Transmitter and the Receiver needs to know these structures

///Commands are sent by the Main module
struct ModuleCommand : CommonTemplate //Max 32bytes. Module command sent by the Main module
{
   ModuleCommand(HempyMessage SequenceID) : CommonTemplate(SequenceID) {}
   ModuleCommand(HempyMessage SequenceID, time_t Time, bool Debug, bool Metric) : CommonTemplate(SequenceID) {}
   time_t Time = 0;
   bool Debug = true;
   bool Metric = true;
};

struct BucketCommand : CommonTemplate //Max 32bytes. Command to control one hempy bucket
{
   BucketCommand(HempyMessage SequenceID) : CommonTemplate(SequenceID) {}
   BucketCommand(HempyMessage SequenceID, bool DisablePump, bool TurnOnPump, bool TurnOffPump, int TimeOutPump, float StartWeight, float StopWeight) : CommonTemplate(SequenceID) {}
   bool DisablePump = false;
   bool TurnOnPump = false;
   bool TurnOffPump = false;
   int TimeOutPump = 0;
   float StartWeight = 0.0;
   float StopWeight = 0.0;
};

///Responses are sent by all other modules, except Main

struct ModuleResponse : CommonTemplate //Max 32bytes. Module response sent back to the Main module
{
   ModuleResponse(HempyMessage SequenceID) : CommonTemplate(SequenceID) {}
   ModuleResponse(HempyMessage SequenceID, bool Status) : CommonTemplate(SequenceID) {}
   bool Status = true;
};

struct BucketResponse : CommonTemplate //Max 32bytes. Response from one hempy bucket
{
   BucketResponse(HempyMessage SequenceID) : CommonTemplate(SequenceID) {}
   BucketResponse(HempyMessage SequenceID, bool PumpOn, bool PumpEnabled, float Weight) : CommonTemplate(SequenceID) {}
   bool PumpOn = false;
   bool PumpEnabled = false;
   float Weight = 0.0;
};

struct DHTResponse : CommonTemplate //Max 32bytes. Response from a DHT sensor
{
   DHTResponse(HempyMessage SequenceID) : CommonTemplate(SequenceID) {}
   DHTResponse(HempyMessage SequenceID, float Temp, float Humidity) : CommonTemplate(SequenceID) {}
   float Temp = 0.0;
   float Humidity = 0.0;
};