#pragma once

#include "TimeLib.h"     ///keeping track of time


///Structs for wireless communication
struct aeroCommand  ///Max 32 bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   time_t Time;
   bool Debug = false;
   bool Metric = true; 
  
};

struct aeroResponse  ///Max 32 bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   
};
