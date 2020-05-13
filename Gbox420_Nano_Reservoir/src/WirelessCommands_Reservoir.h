#pragma once

#include "TimeLib.h"     ///keeping track of time


///Structs for wireless communication
struct reservoirCommand  ///Max 32 bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   time_t Time;
   bool Debug = false;
   bool Metric = true;
   //int TimeOutPump_B1 = 0;   
   //float StartWeightBucket_B1 = 0.0;
   float StopWeightBucket_B1 = 0.0;   
};

struct reservoirResponse  ///Max 32 bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   //PumpState PumpState_B1 = DISABLED;   
};
