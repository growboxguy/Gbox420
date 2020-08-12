#pragma once

#include "TimeLib.h"     ///keeping track of time

///Global constants
const uint8_t NumberOfCommands = 1;  //How many 32byte Command packages are there to exchange
const uint8_t NumberOfResponses = 2; //How many 32byte Responses packages are there to exchange


struct commonAttributes  ///< Shared between Command and Respone packages
{
   uint8_t SequenceID;  ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data, followup packages with increasing SequenceID are exchanged up to the defined numberOfCommands or numberOfRespone global variables.
   
   commonAttributes(uint8_t SequenceID){
      this -> SequenceID = SequenceID;
   }
};

struct hempyResponse : commonAttributes  ///Max 32 bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   hempyResponse() : commonAttributes(1){} 
   PumpState PumpState_B1 = DISABLED;
   float Weight_B1 = 0.0;

   PumpState PumpState_B2 = DISABLED;
   float Weight_B2 = 0.0;
};

struct hempyCommand : commonAttributes  ///Max 32 bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{   

///Structs for wireless communication
   hempyCommand() : commonAttributes(2){}
   time_t Time;
   bool Debug = false;
   bool Metric = true;
   
   //Bucket 1
   bool DisablePump_B1 = false;   
   bool TurnOnPump_B1 = false;   
   bool TurnOffPump_B1 = false;   
   int TimeOutPump_B1 = 0;   
   float StartWeightBucket_B1 = 0.0;
   float StopWeightBucket_B1 = 0.0;
   
   //Bucket 1
   bool DisablePump_B2 = false;
   bool TurnOnPump_B2 = false;
   bool TurnOffPump_B2 = false;
   int TimeOutPump_B2 = 0;
   float StartWeightBucket_B2 = 0.0;
   float StopWeightBucket_B2 = 0.0; 
};

struct hempyCommand_Part2 : commonAttributes  ///Max 32 bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{  
   hempyCommand_Part2() : commonAttributes(1){} 
   time_t Time;
   bool Debug = false;
   bool Metric = true; 
   int Test = 420;
};