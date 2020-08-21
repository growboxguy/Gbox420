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
struct DefaultCommand : commonTemplate  //Max 32bytes. Default command sent by the Main module
{
    DefaultCommand(HempyMessage SequenceID) : commonTemplate(SequenceID){}
    DefaultCommand(HempyMessage SequenceID,time_t Time, bool Debug, bool Metric) : commonTemplate(SequenceID){}
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

struct DefaultResponse  : commonTemplate  //Max 32bytes. Default response sent back to the Main module
{
   DefaultResponse(HempyMessage SequenceID) : commonTemplate(SequenceID){}
   DefaultResponse(HempyMessage SequenceID, bool Status) : commonTemplate(SequenceID){}
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