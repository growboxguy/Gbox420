#pragma once

#include "TimeLib.h"     ///keeping track of time


///Structs for wireless communication
struct hempyCommand  ///Max 32 bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
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

struct hempyResponse  ///Max 32 bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   PumpState PumpState_B1 = DISABLED;
   float Weight_B1 = 0.0;

   PumpState PumpState_B2 = DISABLED;
   float Weight_B2 = 0.0;
};
