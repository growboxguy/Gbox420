/**@file*/
///< GrowBoxGuy - https://sites.google.com/site/growboxguy/
///< Structs for wireless communication using an nRF24L01+ transceiver

///< Defines the stucture of each message exchanged between the Main and the Reservoir module
///< Both the Transmitter (Main module) and the Receiver (Reservoir module) needs to include this
///< Max 32 bytes can be sent in one message

///< The Main module is always the Transmitter, sending a Command defined in ReservoirMessages
///< The Reservoir module is always the Receiver that instantly replies to an incoming message with a pre-cached Acknowledgement(ACK) package
///< ReservoirReset is a special message:
///<   -  Transmitter (Main module) always starts the multi-message exchange with this message
///<   -  When the Receiver gets this message it pre-loads the ACK message for the first "real" message it will receive from the Main module

#pragma once

#include "TimeLib.h" // Keeping track of time

enum ReservoirMessages
{
   ReservoirModuleCommand1, /// First "real" message where the module-level variables are synced: Time, Debug, Serial logging settings
   ReservoirModuleResponse1,
   ReservoirCommand1,
   ReservoirResponse1,
   ReservoirReset /// Special command sent at the start and end of a multi-message exchange.
}; ///< ReservoirReset should always be the last element. An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824

__attribute__((unused)) static const __FlashStringHelper *toText_reservoirSequenceID(uint8_t SequenceID)
{
   switch (SequenceID)
   {
   case ReservoirMessages::ReservoirModuleCommand1:
      return F("RMC1");
      break;
   case ReservoirMessages::ReservoirModuleResponse1:
      return F("RMR1");
      break;
   case ReservoirMessages::ReservoirCommand1:
      return F("RC1");
      break;
   case ReservoirMessages::ReservoirResponse1:
      return F("RR1");
      break;
   case ReservoirMessages::ReservoirReset:
      return F("RR");
      break;
   default:
      return F("?");
      break;
   }
}

struct ReservoirCommonTemplate ///< Shared between Command and Respone packages
{
   ReservoirMessages SequenceID; ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data
   ReservoirCommonTemplate(ReservoirMessages SequenceID)
   {
      this->SequenceID = SequenceID;
   }
};

struct ReservoirModuleCommand : ReservoirCommonTemplate ///< Max 32 bytes. Command sent by the Main module
{
   ReservoirModuleCommand(ReservoirMessages SequenceID) : ReservoirCommonTemplate(SequenceID) {}
   time_t Time = 0;
   bool Debug = true;
   bool Metric = true;
   bool SerialReportDate = true;
   bool SerialReportMemory = true;
   bool SerialReportJSONFriendly = true;
   bool SerialReportJSON = true;
   bool SerialReportWireless = true;
};

struct ReservoirModuleResponse : ReservoirCommonTemplate ///< Max 32 bytes. Response sent back to the Main module
{
   ReservoirModuleResponse(ReservoirMessages SequenceID) : ReservoirCommonTemplate(SequenceID) {}
   bool Status = false;
};

struct ReservoirCommand : ReservoirCommonTemplate ///< Max 32 bytes. Command sent by the Main module
{
   ReservoirCommand(ReservoirMessages SequenceID) : ReservoirCommonTemplate(SequenceID) {}
   bool TareWeight = false;
   bool TareWeightWR = false;
};

struct ReservoirResponse : ReservoirCommonTemplate
{
   ReservoirResponse(ReservoirMessages SequenceID) : ReservoirCommonTemplate(SequenceID) {}
   float PH = 0.0;
   float TDS = 0.0;
   float Weight = 0.0;
   float WeightWR = 0.0;
   float WaterTemperature = 0.0;
   float AirTemperature = 0.0;
   float Humidity = 0.0;
   bool ConfirmTareWeight = false;
   bool ConfirmTareWeightWR = false;
};