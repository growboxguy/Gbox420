/**@file*/
///< GrowBoxGuy - http:///< sites.google.com/site/growboxguy/
///< Structs for wireless communication - Both the Transmitter and the Receiver needs to know these
///< Defines the package stucture of each message exchanged between the Main and the Aero module
///< Max 32 bytes can be sent in a single package

#pragma once

#include "TimeLib.h" // Keeping track of time
///< Structs for wireless communication
enum AeroMessages
{
   AeroModuleCommand1,
   AeroModuleResponse1,
   AeroCommand1,
   AeroResponse1,
   AeroCommand2,
   AeroResponse2,
   AeroReset
}; ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824

static const __FlashStringHelper *toText_aeroSequenceID(uint8_t SequenceID)
{
   switch (SequenceID)
   {
   case AeroMessages::AeroModuleCommand1:
      return F("AMC1");
      break;
   case AeroMessages::AeroModuleResponse1:
      return F("AMR1");
      break;
   case AeroMessages::AeroCommand1:
      return F("AC1");
      break;
   case AeroMessages::AeroResponse1:
      return F("AR1");
      break;
   case AeroMessages::AeroCommand2:
      return F("AC2");
      break;
   case AeroMessages::AeroResponse2:
      return F("AR2");
      break;
   case AeroMessages::AeroReset:
      return F("AR");
      break;
   default:
      return F("?");
      break;
   }
}

///< Both the Transmitter and the Receiver needs to know these structures

struct AeroCommonTemplate ///< Aeroponics wireless template - Shared between Command and Respone packages
{
   AeroCommonTemplate(AeroMessages SequenceID)
   {
      this->SequenceID = SequenceID;
   }
   AeroMessages SequenceID; ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data
};

struct AeroModuleCommand : AeroCommonTemplate ///< Aeroponics wireless module commands
{
   AeroModuleCommand(__attribute__((unused)) AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
   AeroModuleCommand(__attribute__((unused)) AeroMessages SequenceID, __attribute__((unused)) time_t Time, __attribute__((unused)) bool Debug, __attribute__((unused)) bool Metric, __attribute__((unused)) uint16_t SerialReportFrequency, __attribute__((unused)) bool SerialReportDate, __attribute__((unused)) bool SerialReportMemory, __attribute__((unused)) bool SerialReportText, __attribute__((unused)) bool SerialReportJSON, __attribute__((unused)) bool SerialReportWireless) : AeroCommonTemplate(SequenceID) {}
   time_t Time = 0;
   bool Debug = true;
   bool Metric = true;
   uint16_t SerialReportFrequency = 0;
   bool SerialReportDate = true;
   bool SerialReportMemory = true;
   bool SerialReportText = true;
   bool SerialReportJSON = true;
   bool SerialReportWireless = true;
};

struct AeroModuleResponse : AeroCommonTemplate ///< Aeroponics wireless module response
{
   AeroModuleResponse(__attribute__((unused)) AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
   AeroModuleResponse(__attribute__((unused)) AeroMessages SequenceID, __attribute__((unused)) bool Status) : AeroCommonTemplate(SequenceID) {}
   bool Status = true;
};

struct AeroCommand_P1 : AeroCommonTemplate ///< Aeroponics wireless commands - Part1
{
   AeroCommand_P1(__attribute__((unused)) AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
   AeroCommand_P1(__attribute__((unused)) AeroMessages SequenceID, __attribute__((unused)) bool DayMode, __attribute__((unused)) bool SprayEnabled, __attribute__((unused)) bool SprayDisabled, __attribute__((unused)) bool SprayNow, __attribute__((unused)) bool SprayOff, __attribute__((unused)) float Duration, __attribute__((unused)) int DayInterval, __attribute__((unused)) int NightInterval, __attribute__((unused)) float MinPressure, __attribute__((unused)) float MaxPressure) : AeroCommonTemplate(SequenceID) {}
   bool DayMode = true;
   bool SprayEnabled = false;
   bool SprayDisabled = false;
   bool SprayNow = false;
   bool SprayOff = false;
   float Duration = 0.0;
   int DayInterval = 0;
   int NightInterval = 0;
   float MinPressure = 0.0;
   float MaxPressure = 0.0;
};

struct AeroCommand_P2 : AeroCommonTemplate ///< Aeroponics wireless commands - Part2
{
   AeroCommand_P2(__attribute__((unused)) AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
   AeroCommand_P2(__attribute__((unused)) AeroMessages SequenceID, __attribute__((unused)) bool PumpOn, __attribute__((unused)) bool PumpOff, __attribute__((unused)) bool PumpDisable, __attribute__((unused)) bool MixReservoir, __attribute__((unused)) bool RefillPressureTank, __attribute__((unused)) bool DrainPressureTank, __attribute__((unused)) bool TareWeight, __attribute__((unused)) uint8_t PumpSpeed, __attribute__((unused)) int PumpPrimingTime, __attribute__((unused)) uint16_t PumpTimeOut) : AeroCommonTemplate(SequenceID) {}

   bool PumpOn = false;
   bool PumpOff = false;
   bool PumpDisable = false;
   bool MixReservoir = false;
   bool RefillPressureTank = false;
   bool DrainPressureTank = false;
   bool TareWeight = false;
   uint8_t PumpSpeed = 0;
   int PumpPrimingTime = 0;
   uint16_t PumpTimeOut = 0;
};

struct AeroResponse_P1 : AeroCommonTemplate ///< Aeroponics wireless response - Part1
{
   AeroResponse_P1(__attribute__((unused)) AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
   AeroResponse_P1(__attribute__((unused)) AeroMessages SequenceID, __attribute__((unused)) PressurePumpStates AeroState, __attribute__((unused)) bool PressureTankPresent, __attribute__((unused)) bool SprayEnabled, __attribute__((unused)) float Pressure, __attribute__((unused)) PressurePumpStates PumpState, __attribute__((unused)) float LastSprayPressure, __attribute__((unused)) float Weight) : AeroCommonTemplate(SequenceID) {}
   AeroTankStates AeroState = AeroTankStates::DISABLED;
   bool PressureTankPresent = false;
   bool SprayEnabled = false;
   float Pressure = 0.0;
   PressurePumpStates PumpState = PressurePumpStates::DISABLED;
   float LastSprayPressure = 0.0; // Used only without pressure tank. last spray pressure
   float Weight = 0.0;
};

struct AeroResponse_P2 : AeroCommonTemplate ///< Aeroponics wireless response - Part2
{
   AeroResponse_P2(__attribute__((unused)) AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
};