#pragma once

#include "TimeLib.h"     ///< keeping track of time
///Structs for wireless communication

///Global constants
const uint8_t NumberOfHempyMessage = 2;  //How many packages are there to exchange (max 32byte / package) 
const uint8_t NumberOfResponses = 1; //How many 32byte Responses packages are there to exchange

enum HempyMessage { DefaultCommand,DefaultResponse,Bucket1Command,Bucket2Command,Bucket1Response,Bucket2Response,DHTResponse};  ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824


struct commonTemplate  ///< Shared between Command and Respone packages
{
   uint8_t SequenceID;  ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data
   
   commonTemplate(uint8_t SequenceID){
      this -> SequenceID = SequenceID;
   }
};

///< Both the Transmitter and the Receiver needs to know these structures

///< Commands are sent by the Main module
struct DefaultCommand : commonTemplate  //Max 32bytes. Default message sent by the Main module
{
    DefaultCommand(HempyMessage SequenceID) : commonTemplate(SequenceID){}
    DefaultCommand(HempyMessage SequenceID,time_t Time, bool Debug, bool Metric) : commonTemplate(SequenceID){}
    time_t Time; 
    bool Debug;
    bool Metric;
};

struct BucketCommand : commonTemplate  //Max 32bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
    BucketCommand(HempyMessage SequenceID) : commonTemplate(SequenceID){}
    BucketCommand(HempyMessage SequenceID, bool DisablePump, bool TurnOnPump, bool TurnOffPump, int TimeOutPump, float StartWeight, float StopWeight) : commonTemplate(SequenceID){}   
    bool DisablePump;
    bool TurnOnPump;
    bool TurnOffPump;
    int TimeOutPump;
    float StartWeight;
    float StopWeight;
};

///< Responses are sent by all other modules, except Main  

struct DefaultResponse  : commonTemplate  //Max 32bytes. Template of the response sent back by the Receiver. Both Transmitter and Receiver needs to know this structure
{
   DefaultResponse() : commonTemplate(HempyMessage::DefaultResponse){}
   DefaultResponse(bool Status) : commonTemplate(HempyMessage::DefaultResponse){}
   bool Status; 
   
};

struct Bucket1Response  : commonTemplate  //Max 32bytes. Template of the response sent back by the Receiver. Both Transmitter and Receiver needs to know this structure
{
   Bucket1Response() : commonTemplate(HempyMessage::Bucket1Response){}
   Bucket1Response(bool PumpOn_B1, bool PumpEnabled_B1, float Weight_B1, bool PumpOn_B2, bool PumpEnabled_B2, float Weight_B2, float Temp, float Humidity) : commonTemplate(HempyMessage::Bucket1Response){}
   bool PumpOn_B1; 
   bool PumpEnabled_B1;
   float Weight_B1;

   bool PumpOn_B2;
   bool PumpEnabled_B2; 
   float Weight_B2;
   
   float Temp;
   float Humidity;
};

struct Bucket2Response  : commonTemplate  //Max 32bytes. Template of the response sent back by the Receiver. Both Transmitter and Receiver needs to know this structure
{
   Bucket2Response() : commonTemplate(HempyMessage::Bucket2Response){}
   Bucket2Response(bool PumpOn_B1, bool PumpEnabled_B1, float Weight_B1, bool PumpOn_B2, bool PumpEnabled_B2, float Weight_B2, float Temp, float Humidity) : commonTemplate(HempyMessage::Bucket2Response){}
   bool PumpOn_B1; 
   bool PumpEnabled_B1;
   float Weight_B1;

   bool PumpOn_B2;
   bool PumpEnabled_B2; 
   float Weight_B2;
   
   float Temp;
   float Humidity;
};

struct DHTResponse  : commonTemplate  //Max 32bytes. Template of the response sent back by the Receiver. Both Transmitter and Receiver needs to know this structure
{
   DHTResponse() : commonTemplate(HempyMessage::DHTResponse){}
   DHTResponse(bool PumpOn_B1, bool PumpEnabled_B1, float Weight_B1, bool PumpOn_B2, bool PumpEnabled_B2, float Weight_B2, float Temp, float Humidity) : commonTemplate(HempyMessage::DHTResponse){}
   bool PumpOn_B1; 
   bool PumpEnabled_B1;
   float Weight_B1;

   bool PumpOn_B2;
   bool PumpEnabled_B2; 
   float Weight_B2;
   
   float Temp;
   float Humidity;
};