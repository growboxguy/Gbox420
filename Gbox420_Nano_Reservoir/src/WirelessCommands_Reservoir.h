#pragma once

#include "TimeLib.h"     ///keeping track of time


///Structs for wireless communication
struct reservoirCommand  ///Max 32 bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   time_t Time;
   bool Debug = false;
   bool Metric = true;
};

struct reservoirResponse  ///Max 32 bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   float PH = 0.0;
   float Weight = 0.0;
   float WaterTemperature = 0.0;
   float AirTemperature = 0.0;
   float Humidity = 0.0;    
};
