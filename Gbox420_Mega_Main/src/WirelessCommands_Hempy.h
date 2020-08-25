#pragma once

#include "TimeLib.h"     ///< keeping track of time
///Structs for wireless communication

///Global constants
enum HempyMessages { HempyModule1Command,HempyModule1Response,HempyBucket1Command,HempyBucket1Response,HempyBucket2Command,HempyBucket2Response,HempyGetNext};  ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824

static const __FlashStringHelper* toText_hempySequenceID(uint8_t SequenceID) 
{
   switch (SequenceID) 
   {
      case HempyMessages::HempyModule1Command: return F("HempyModule1Command"); break;
      case HempyMessages::HempyModule1Response: return F("HempyModule1Response"); break;
      case HempyMessages::HempyBucket1Command: return F("HempyBucket1Command"); break;
      case HempyMessages::HempyBucket1Response: return F("HempyBucket1Response"); break;
      case HempyMessages::HempyBucket2Command: return F("HempyBucket2Command"); break;
      case HempyMessages::HempyBucket2Response: return F("HempyBucket2Response"); break;
      case HempyMessages::HempyGetNext: return F("HempyGetNext"); break;
      default : return F("UNKNOWN"); break;
   }
}

///< Both the Transmitter and the Receiver needs to know these structures
                                                                                                                                         ///< HempyGetNext should always be the last element
struct HempyCommonTemplate  ///< Shared between Command and Respone packages
{
   HempyMessages SequenceID;  ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data
   HempyCommonTemplate(HempyMessages SequenceID){
      this -> SequenceID = SequenceID;
   }
};

///< Commands are sent by the Main module
struct HempyModuleCommand : HempyCommonTemplate  //Max 32bytes. Module command sent by the Main module
{
    HempyModuleCommand(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID){}
    HempyModuleCommand(__attribute__((unused)) HempyMessages SequenceID,__attribute__((unused)) time_t Time,__attribute__((unused)) bool Debug,__attribute__((unused)) bool Metric) : HempyCommonTemplate(SequenceID){}
    time_t Time = 0; 
    bool Debug = true;
    bool Metric = true;
};

struct HempyModuleResponse  : HempyCommonTemplate  //Max 32bytes. Module response sent back to the Main module
{
   HempyModuleResponse(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID){}
   HempyModuleResponse(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) bool Status) : HempyCommonTemplate(SequenceID){}
   bool Status = true;   
};

struct HempyBucketCommand : HempyCommonTemplate  //Max 32bytes. Command to control one hempy bucket
{
    HempyBucketCommand(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID){}
    HempyBucketCommand(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) bool DisablePump, __attribute__((unused)) bool TurnOnPump, __attribute__((unused)) bool TurnOffPump, __attribute__((unused)) int TimeOutPump, __attribute__((unused)) float StartWeight, __attribute__((unused)) float StopWeight) : HempyCommonTemplate(SequenceID){}   
    bool DisablePump = false;
    bool TurnOnPump = false;
    bool TurnOffPump = false;
    int TimeOutPump = 0;
    float StartWeight = 0.0;
    float StopWeight = 0.0;
};

struct HempyBucketResponse  : HempyCommonTemplate  //Max 32bytes. Response from one hempy bucket
{
   HempyBucketResponse(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID){}
   HempyBucketResponse(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) bool PumpOn,__attribute__((unused))  bool PumpEnabled,__attribute__((unused))  float Weight) : HempyCommonTemplate(SequenceID){}
   PumpStates PumpState = DISABLED;
   float Weight = 0.0;
};