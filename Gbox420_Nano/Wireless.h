#pragma once

///Structs for wireless communication
struct commandTemplate  ///Max 32bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   bool pump1Enabled; 
   bool pump2Enabled;
   bool pump1Stop; 
   bool pump2Stop;
   bool bucket1StartWatering; 
   bool bucket2StartWatering;
   float bucket1StartWeight;
   float bucket1StopWeight;
   float bucket2StartWeight;
   float bucket2StopWeight;
};
extern struct commandTemplate Command;  //Variable where the actual command values will get stored

struct responseTemplate  ///Max 32bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   bool pump1Enabled; 
   bool pump2Enabled;
   bool pump1State; 
   bool pump2State;
   float bucket1Weight;
   float bucket2Weight;
   float temp;
   float humidity;
};
extern struct responseTemplate Response;  ///Response sent back in the Acknowledgement after receiving a command from the Transmitter
//extern struct responseTemplate Response = {1,1,0,1,4.20,1.23};  ///Response sent back in the Acknowledgement after receiving a command from the Transmitter
