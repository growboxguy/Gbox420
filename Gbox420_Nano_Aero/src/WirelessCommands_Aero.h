#pragma once

#include "TimeLib.h"     ///keeping track of time


///Structs for wireless communication
struct aeroCommand  ///Max 32 bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   time_t Time;
   bool Debug = false;
   bool Metric = true; 

   //Aero tank - Commands for both Aeroponics versions (With or without a pressure tank)
   bool SprayEnable = false;
   bool SprayDisable = false;
   bool SprayNow = false;
   bool SprayOff = false;
   int SprayInterval = 0 ;
   int SprayDuration = 0;
   bool PumpOn = false;
   bool PumpOff = false;
   bool PumpDisable = false;
   int PumpPriming = 0;
   int PumpTimeOut = 0;   
   bool MixReservoir = false;    

   //Aero no tank - Aeroponics without a pressure tank
   bool BypassOn = false;
   bool BypassOff = false; 
};

struct aeroResponse  ///Max 32 bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   bool SprayEnabled = false;
   float Pressure = 0.0;
   bool PumpOn = false;
   bool PumpEnabled = false;
   bool BypassOn = false;
   float LastSP = 0.0; ///< Used only without pressure tank. last spray pressure

};
