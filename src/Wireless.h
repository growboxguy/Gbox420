#pragma once

///Structs for wireless communication
struct commandTemplate  ///Max 32bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   time_t Time;
   
   bool DisablePump1;   
   bool TurnOnPump1;   
   bool TurnOffPump1;   
   int TimeOutPump1;   
   float StartWeightBucket1;
   float StopWeightBucket1;
   
   bool DisablePump2;
   bool TurnOnPump2;
   bool TurnOffPump2;
   int TimeOutPump2;
   float StartWeightBucket2;
   float StopWeightBucket2;
};
extern struct commandTemplate Command;  //Variable where the actual command values will get stored

struct responseTemplate  ///Max 32bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   bool Pump1Enabled; 
   bool Pump2Enabled;
   bool Pump1State; 
   bool Pump2State;
   float Bucket1Weight;
   float Bucket2Weight;
   float Temp;
   float Humidity;
};
extern struct responseTemplate Response;  ///Response sent back in the Acknowledgement after receiving a command from the Transmitter
//extern struct responseTemplate Response = {1,1,0,1,4.20,1.23};  ///Response sent back in the Acknowledgement after receiving a command from the Transmitter
