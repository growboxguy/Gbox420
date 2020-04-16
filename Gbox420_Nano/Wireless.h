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
struct commandTemplate ReceivedCommand;  //Variable where the actual command values will get stored

struct responseTemplate  ///Max 32bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   bool pump1Enabled; 
   bool pump2Enabled;
   bool pump1State; 
   bool pump2State;
   float bucket1Weight;
   float bucket2Weight;
};
struct responseTemplate FakeResponse = {1,1,0,1,4.20,1.23};  ///Fake response sent back in the Acknowledgement after receiving a command from the Transmitter
