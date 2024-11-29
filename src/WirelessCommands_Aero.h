/**@file*/
///< GrowBoxGuy - https://sites.google.com/site/growboxguy/
///< Structs for wireless communication using an nRF24L01+ transceiver

///< Defines the stucture of each message exchanged between the Main and the Aero module
///< Both the Transmitter (Main module) and the Receiver (Aero module) needs to include this
///< Max 32 bytes can be sent in one message

///< The Main module is always the Transmitter, sending a Command defined in AeroMessages
///< The Aero module is always the Receiver that instantly replies to an incoming message with a pre-cached Acknowledgement(ACK) package
///< AeroReset is a special message:
///<   -  Transmitter (Main module) always starts the multi-message exchange with this message
///<   -  When the Receiver gets this message it pre-loads the ACK message for the first "real" message it will receive from the Main module

#pragma once

#include "TimeLib.h" // Keeping track of time

///< Structs for wireless communication
enum AeroMessages
{
   AeroModuleCommand1, /// First "real" message where the module-level variables are synced, send by the Main module: Time, Debug, Serial logging settings
   AeroModuleResponse1,
   AeroCommand1,
   AeroResponse1,
   AeroCommand2,
   AeroResponse2,
   AeroReset /// Special command sent at the start and end of a multi-message exchange.
}; ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824

__attribute__((unused)) static const __FlashStringHelper *toText_aeroSequenceID(uint8_t SequenceID)
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
   AeroModuleCommand(AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
   time_t Time = 0;
   bool Debug = true;
   bool Metric = true;
   bool SerialReportDate = true;
   bool SerialReportMemory = true;
   bool SerialReportJSONFriendly = true;
   bool SerialReportJSON = true;
   bool SerialReportWireless = true;
};

struct AeroModuleResponse : AeroCommonTemplate ///< Aeroponics wireless module response
{
   AeroModuleResponse(AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
   bool Status = false;
};

struct AeroCommand_P1 : AeroCommonTemplate ///< Aeroponics wireless commands - Part1
{
   AeroCommand_P1(AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
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

struct AeroResponse_P1 : AeroCommonTemplate ///< Aeroponics wireless response - Part1
{
   AeroResponse_P1(AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
   bool ConfirmSprayEnabled = false;
   bool ConfirmSprayDisabled = false;
   bool ConfirmSprayNow = false;
   bool ConfirmSprayOff = false;
   int AeroState = 0;
   bool PressureTankPresent = false;
   bool SprayEnabled = false;
   float Pressure = 0.0;
   PressurePumpStates PumpState = PressurePumpStates::DISABLED;
   float LastSprayPressure = 0.0; // Used only without pressure tank. last spray pressure
   float Weight = 0.0;
};

struct AeroCommand_P2 : AeroCommonTemplate ///< Aeroponics wireless commands - Part2
{
   AeroCommand_P2(AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
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

struct AeroResponse_P2 : AeroCommonTemplate ///< Aeroponics wireless response - Part2
{
   AeroResponse_P2(AeroMessages SequenceID) : AeroCommonTemplate(SequenceID) {}
   bool ConfirmPumpOn = false;
   bool ConfirmPumpOff = false;
   bool ConfirmPumpDisable = false;
   bool ConfirmMixReservoir = false;
   bool ConfirmRefillPressureTank = false;
   bool ConfirmDrainPressureTank = false;
   bool ConfirmTareWeight = false;
};