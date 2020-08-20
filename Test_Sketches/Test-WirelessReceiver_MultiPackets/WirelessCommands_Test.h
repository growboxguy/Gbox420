#pragma once

#include "TimeLib.h"     ///< keeping track of time
///Structs for wireless communication

struct commonTemplate  ///< Shared between Command and Respone packages
{
   uint8_t SequenceID;  ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data, followup packages with increasing SequenceID are exchanged up to the defined numberOfCommands or numberOfRespone global variables.
   
   commonTemplate(uint8_t SequenceID){
      this -> SequenceID = SequenceID;
   }
};

struct Command1 : commonTemplate  //Max 32bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
    Command1() : commonTemplate(0){}
    Command1(time_t Time, bool DisablePump_B1, bool TurnOnPump_B1, bool TurnOffPump_B1, int TimeOutPump_B1, float StartWeightBucket_B1, float StopWeightBucket_B1) : commonTemplate(0){}
    time_t Time;     
    bool DisablePump_B1;
    bool TurnOnPump_B1;
    bool TurnOffPump_B1;
    int TimeOutPump_B1;
    float StartWeightBucket_B1;
    float StopWeightBucket_B1;
};

struct Command2 : commonTemplate  //Max 32bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
    Command2() : commonTemplate(1){}
    Command2(bool DisablePump_B2, bool TurnOnPump_B2, bool TurnOffPump_B2, int TimeOutPump_B2, float StartWeightBucket_B2, float StopWeightBucket_B2) : commonTemplate(1){}
    bool DisablePump_B2;
    bool TurnOnPump_B2;
    bool TurnOffPump_B2;
    int TimeOutPump_B2;
    float StartWeightBucket_B2;
    float StopWeightBucket_B2;
};

struct Response1  : commonTemplate  //Max 32bytes. Template of the response sent back by the Receiver. Both Transmitter and Receiver needs to know this structure
{
   Response1() : commonTemplate(2){}
   Response1(bool PumpOn_B1, bool PumpEnabled_B1, float Weight_B1, bool PumpOn_B2, bool PumpEnabled_B2, float Weight_B2, float Temp, float Humidity) : commonTemplate(2){}
   bool PumpOn_B1; 
   bool PumpEnabled_B1;
   float Weight_B1;

   bool PumpOn_B2;
   bool PumpEnabled_B2; 
   float Weight_B2;
   
   float Temp;
   float Humidity;
};