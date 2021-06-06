/**@file*/
///< GrowBoxGuy - http:///< sites.google.com/site/growboxguy/
///< Structs for wireless communication - Both the Transmitter and the Receiver needs to know these
///< Defines the package stucture of each message exchanged between the Main and the Dev module
///< Max 32 bytes can be sent in a single package

#pragma once

#include "TimeLib.h" // Keeping track of time
///< Structs for wireless communication
enum DevMessages
{
   DevModuleCommand1,
   DevModuleResponse1,
   DevCommand1,
   DevResponse1,
   DevCommand2,
   DevResponse2,
   DevReset
}; ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824

static const __FlashStringHelper *toText_devSequenceID(uint8_t SequenceID)
{
   switch (SequenceID)
   {
   case DevMessages::DevModuleCommand1:
      return F("DMC1");
      break;
   case DevMessages::DevModuleResponse1:
      return F("DMR1");
      break;
   case DevMessages::DevCommand1:
      return F("DC1");
      break;
   case DevMessages::DevResponse1:
      return F("DR1");
      break;
   case DevMessages::DevCommand2:
      return F("DC2");
      break;
   case DevMessages::DevResponse2:
      return F("DR2");
      break;
   case DevMessages::DevReset:
      return F("DR");
      break;
   default:
      return F("?");
      break;
   }
}

///< Both the Transmitter and the Receiver needs to know these structures

struct DevCommonTemplate ///< Devponics wireless template - Shared between Command and Respone packages
{
   DevCommonTemplate(DevMessages SequenceID)
   {
      this->SequenceID = SequenceID;
   }
   DevMessages SequenceID; ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data
};

struct DevModuleCommand : DevCommonTemplate ///< Devponics wireless module commands
{
   DevModuleCommand(__attribute__((unused)) DevMessages SequenceID) : DevCommonTemplate(SequenceID) {}
   DevModuleCommand(__attribute__((unused)) DevMessages SequenceID, __attribute__((unused)) time_t Time, __attribute__((unused)) bool Debug, __attribute__((unused)) bool Metric, __attribute__((unused)) uint16_t SerialReportFrequency, __attribute__((unused)) bool SerialReportDate, __attribute__((unused)) bool SerialReportMemory, __attribute__((unused)) bool SerialReportJSONFriendly, __attribute__((unused)) bool SerialReportJSON, __attribute__((unused)) bool SerialReportWireless) : DevCommonTemplate(SequenceID) {}
   time_t Time = 0;
   bool Debug = true;
   bool Metric = true;
   uint16_t SerialReportFrequency = 0;
   bool SerialReportDate = true;
   bool SerialReportMemory = true;
   bool SerialReportJSONFriendly = true;
   bool SerialReportJSON = true;
   bool SerialReportWireless = true;
};

struct DevModuleResponse : DevCommonTemplate ///< Devponics wireless module response
{
   DevModuleResponse(__attribute__((unused)) DevMessages SequenceID) : DevCommonTemplate(SequenceID) {}
   DevModuleResponse(__attribute__((unused)) DevMessages SequenceID, __attribute__((unused)) bool Status) : DevCommonTemplate(SequenceID) {}
   bool Status = false;
};

struct DevCommand_P1 : DevCommonTemplate ///< Devponics wireless commands - Part1
{
   DevCommand_P1(__attribute__((unused)) DevMessages SequenceID) : DevCommonTemplate(SequenceID) {}
   DevCommand_P1(__attribute__((unused)) DevMessages SequenceID, __attribute__((unused)) bool DayMode, __attribute__((unused)) bool SprayEnabled, __attribute__((unused)) bool SprayDisabled, __attribute__((unused)) bool SprayNow, __attribute__((unused)) bool SprayOff, __attribute__((unused)) float Duration, __attribute__((unused)) int DayInterval, __attribute__((unused)) int NightInterval, __attribute__((unused)) float MinPressure, __attribute__((unused)) float MaxPressure) : DevCommonTemplate(SequenceID) {}
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

struct DevCommand_P2 : DevCommonTemplate ///< Devponics wireless commands - Part2
{
   DevCommand_P2(__attribute__((unused)) DevMessages SequenceID) : DevCommonTemplate(SequenceID) {}
   DevCommand_P2(__attribute__((unused)) DevMessages SequenceID, __attribute__((unused)) bool PumpOn, __attribute__((unused)) bool PumpOff, __attribute__((unused)) bool PumpDisable, __attribute__((unused)) bool MixReservoir, __attribute__((unused)) bool RefillPressureTank, __attribute__((unused)) bool DrainPressureTank, __attribute__((unused)) bool TareWeight, __attribute__((unused)) uint8_t PumpSpeed, __attribute__((unused)) int PumpPrimingTime, __attribute__((unused)) uint16_t PumpTimeOut) : DevCommonTemplate(SequenceID) {}

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

struct DevResponse_P1 : DevCommonTemplate ///< Devponics wireless response - Part1
{
   DevResponse_P1(__attribute__((unused)) DevMessages SequenceID) : DevCommonTemplate(SequenceID) {}
   DevResponse_P1(__attribute__((unused)) DevMessages SequenceID, __attribute__((unused)) int DevState, __attribute__((unused)) bool PressureTankPresent, __attribute__((unused)) bool SprayEnabled, __attribute__((unused)) float Pressure, __attribute__((unused)) PressurePumpStates PumpState, __attribute__((unused)) float LastSprayPressure, __attribute__((unused)) float Weight) : DevCommonTemplate(SequenceID) {}
   int DevState = 0;
   bool PressureTankPresent = false;
   bool SprayEnabled = false;
   float Pressure = 0.0;
   PressurePumpStates PumpState = PressurePumpStates::DISABLED;
   float LastSprayPressure = 0.0; // Used only without pressure tank. last spray pressure
   float Weight = 0.0;
};

struct DevResponse_P2 : DevCommonTemplate ///< Devponics wireless response - Part2
{
   DevResponse_P2(__attribute__((unused)) DevMessages SequenceID) : DevCommonTemplate(SequenceID) {}
};