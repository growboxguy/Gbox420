/**@file*/
///GrowBoxGuy - http:///sites.google.com/site/growboxguy/
///Structs for wireless communication - Both the Transmitter and the Receiver needs to know these
///Defines the package stucture of each message exchanged between the Main and the Aero module
///Max 32 bytes can be sent in a single package

#pragma once

#include "TimeLib.h"     ///< keeping track of time
///Structs for wireless communication
enum AeroMessages { AeroModuleCommand1,AeroModuleResponse1,AeroCommand1,AeroResponse1,AeroReset};  ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824

static const __FlashStringHelper* toText_aeroSequenceID(uint8_t SequenceID)
{
   switch (SequenceID) 
   {
      case AeroMessages::AeroModuleCommand1: return F("AeroModuleCommand1"); break;
      case AeroMessages::AeroModuleResponse1: return F("AeroModuleResponse1"); break;
      case AeroMessages::AeroCommand1: return F("AeroCommand1"); break;
      case AeroMessages::AeroResponse1: return F("AeroResponse1"); break;
      case AeroMessages::AeroReset: return F("AeroReset"); break;
      default : return F("UNKNOWN"); break;
   }
}

///< Both the Transmitter and the Receiver needs to know these structures

struct AeroCommonTemplate  ///< Shared between Command and Respone packages
{
   AeroCommonTemplate(AeroMessages SequenceID){
      this -> SequenceID = SequenceID;
   }
   AeroMessages SequenceID;  ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data
};

struct AeroModuleCommand : AeroCommonTemplate  //Max 32bytes. Module command sent by the Main module
{
    AeroModuleCommand(__attribute__((unused)) AeroMessages SequenceID) : AeroCommonTemplate(SequenceID){}
    AeroModuleCommand(__attribute__((unused)) AeroMessages SequenceID,__attribute__((unused)) time_t Time,__attribute__((unused)) bool Debug,__attribute__((unused)) bool Metric) : AeroCommonTemplate(SequenceID){}
    time_t Time = 0; 
    bool Debug = true;
    bool Metric = true;
};

struct AeroModuleResponse  : AeroCommonTemplate  //Max 32bytes. Module response sent back to the Main module
{
   AeroModuleResponse(__attribute__((unused)) AeroMessages SequenceID) : AeroCommonTemplate(SequenceID){}
   AeroModuleResponse(__attribute__((unused)) AeroMessages SequenceID, __attribute__((unused)) bool Status) : AeroCommonTemplate(SequenceID){}
   bool Status = true;   
};

struct AeroCommand : AeroCommonTemplate  ///Max 32 bytes. Commands for both Aeroponics versions (With or without a pressure tank)
{
   AeroCommand(__attribute__((unused)) AeroMessages SequenceID) : AeroCommonTemplate(SequenceID){}
   AeroCommand(__attribute__((unused)) AeroMessages SequenceID, __attribute__((unused)) bool DayMode, __attribute__((unused)) bool SprayEnabled, __attribute__((unused)) bool SprayDisabled, __attribute__((unused)) bool SprayNow, __attribute__((unused)) bool SprayOff, __attribute__((unused)) int DayInterval, __attribute__((unused)) int DayDuration,  __attribute__((unused)) int NightInterval, __attribute__((unused)) int NightDuration, __attribute__((unused)) bool PumpOn, __attribute__((unused)) bool PumpOff, __attribute__((unused)) bool PumpDisable, __attribute__((unused)) uint8_t PumpSpeed, __attribute__((unused)) int PumpPrimingTime, __attribute__((unused)) uint16_t PumpTimeOut, __attribute__((unused)) bool MixReservoir, __attribute__((unused)) bool RefillPressureTank, __attribute__((unused)) bool TareWeight, __attribute__((unused)) float MinPressure, __attribute__((unused)) float MaxPressure) : AeroCommonTemplate(SequenceID){}   
   bool DayMode = true;
   bool SprayEnabled = false;
   bool SprayDisabled = false;
   bool SprayNow = false;
   bool SprayOff = false;
   bool PumpOn = false;
   bool PumpOff = false;
   bool PumpDisable = false;
   bool MixReservoir = false;
   bool RefillPressureTank = false;
   bool TareWeight = false;
   uint8_t PumpSpeed = 0;
   int PumpPrimingTime = 0;
   uint16_t PumpTimeOut = 0;  
   int DayInterval = 0;
   int DayDuration = 0;
   int NightInterval = 0;
   int NightDuration = 0;
   float MinPressure = 0.0;
   float MaxPressure = 0.0;    
};

struct AeroResponse : AeroCommonTemplate  ///Max 32 bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   AeroResponse(__attribute__((unused)) AeroMessages SequenceID) : AeroCommonTemplate(SequenceID){}
   AeroResponse(__attribute__((unused)) AeroMessages SequenceID, __attribute__((unused)) bool PressureTankPresent, __attribute__((unused)) bool SprayEnabled, __attribute__((unused)) float Pressure, __attribute__((unused)) PumpStates State, __attribute__((unused)) float LastSprayPressure, __attribute__((unused)) float Weight) : AeroCommonTemplate(SequenceID){}   
   bool PressureTankPresent = false;
   bool SprayEnabled = false;
   float Pressure = 0.0;
   PumpStates State = DISABLED;
   float LastSprayPressure = 0.0; ///< Used only without pressure tank. last spray pressure
   float Weight = 0.0;
};