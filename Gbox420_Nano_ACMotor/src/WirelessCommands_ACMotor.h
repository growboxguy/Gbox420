/**@file*/
///< GrowBoxGuy - https://sites.google.com/site/growboxguy/
///< Structs for wireless communication using an nRF24L01+ transceiver

///< Defines the stucture of each message exchanged between the Main and the ACMotor module
///< Both the Transmitter (Main module) and the Receiver (ACMotor module) needs to include this
///< Max 32 bytes can be sent in one message

///< The Main module is always the Transmitter, sending a Command defined in ACMotorMessages
///< The ACMotor module is always the Receiver that instantly replies to an incoming message with a pre-cached Acknowledgement(ACK) package
///< ACMotorReset is a special message:
///<   -  Transmitter (Main module) always starts the multi-message exchange with this message
///<   -  When the Receiver gets this message it pre-loads the ACK message for the first "real" message it will receive from the Main module

#pragma once

#include "TimeLib.h" // Keeping track of time

enum ACMotorMessages
{
   ACMotorModuleCommand1, /// First "real" message where the module-level variables are synced: Time, Debug, Serial logging settings
   ACMotorModuleResponse1,
   ACMotorCommand1,
   ACMotorResponse1,
   ACMotorReset /// Special command sent at the start and end of a multi-message exchange.
};              ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824

__attribute__((unused)) static const __FlashStringHelper *toText_ACMotorSequenceID(uint8_t SequenceID)
{
   switch (SequenceID)
   {
   case ACMotorMessages::ACMotorModuleCommand1:
      return F("MMC1");
      break;
   case ACMotorMessages::ACMotorModuleResponse1:
      return F("MMR1");
      break;
   case ACMotorMessages::ACMotorCommand1:
      return F("MC1");
      break;
   case ACMotorMessages::ACMotorResponse1:
      return F("MR1");
      break;
   case ACMotorMessages::ACMotorReset:
      return F("MR");
      break;
   default:
      return F("?");
      break;
   }
}
///< ACMotorReset should always be the last element
struct ACMotorCommonTemplate ///< ACMotor wireless template - Shared between Command and Respone packages
{
   ACMotorMessages SequenceID; ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data
   ACMotorCommonTemplate(ACMotorMessages SequenceID)
   {
      this->SequenceID = SequenceID;
   }
};

struct ACMotorModuleCommand : ACMotorCommonTemplate ///< ACMotor module wireless command
{
   ACMotorModuleCommand(__attribute__((unused)) ACMotorMessages SequenceID) : ACMotorCommonTemplate(SequenceID) {}
   ACMotorModuleCommand(__attribute__((unused)) ACMotorMessages SequenceID, __attribute__((unused)) time_t Time, __attribute__((unused)) bool Debug, __attribute__((unused)) bool Metric, __attribute__((unused)) uint16_t SerialReportFrequency, __attribute__((unused)) bool SerialReportDate, __attribute__((unused)) bool SerialReportMemory, __attribute__((unused)) bool SerialReportJSONFriendly, __attribute__((unused)) bool SerialReportJSON, __attribute__((unused)) bool SerialReportWireless) : ACMotorCommonTemplate(SequenceID) {}
   time_t Time = 0;
   bool Debug = true;
   bool Metric = true;
   uint16_t SerialReportFrequency = 15;
   bool SerialReportDate = true;
   bool SerialReportMemory = true;
   bool SerialReportJSONFriendly = true;
   bool SerialReportJSON = true;
   bool SerialReportWireless = true;
};

struct ACMotorModuleResponse : ACMotorCommonTemplate ///< ACMotor module wireless response
{
   ACMotorModuleResponse(__attribute__((unused)) ACMotorMessages SequenceID) : ACMotorCommonTemplate(SequenceID) {}
   ACMotorModuleResponse(__attribute__((unused)) ACMotorMessages SequenceID, __attribute__((unused)) bool Status) : ACMotorCommonTemplate(SequenceID) {}
   bool Status = false;
};

struct ACMotorCommand : ACMotorCommonTemplate ///< ACMotor wireless command
{
   ACMotorCommand(__attribute__((unused)) ACMotorMessages SequenceID) : ACMotorCommonTemplate(SequenceID) {}
   ACMotorCommand(__attribute__((unused)) ACMotorMessages SequenceID, __attribute__((unused)) bool Forward, __attribute__((unused)) bool Backward, __attribute__((unused)) bool Stop, __attribute__((unused)) uint8_t Speed) : ACMotorCommonTemplate(SequenceID) {}
   bool Forward = false;  ///< Flag to signal a request to spin the AC motor forward
   bool Backward = false; ///< Flag to signal a request to spin the AC motor backward
   bool Stop = false;     ///< Flag to signal a request to stop the AC motor
   uint8_t Speed = 0;     ///< Set motor speed %  (between 0%-100%)
};

struct ACMotorResponse : ACMotorCommonTemplate ///< ACMotor  wireless response
{
   ACMotorResponse(__attribute__((unused)) ACMotorMessages SequenceID) : ACMotorCommonTemplate(SequenceID) {}
   ACMotorResponse(__attribute__((unused)) ACMotorMessages SequenceID, __attribute__((unused)) ACMotorStates ACMotorState, __attribute__((unused)) bool ConfirmForward, __attribute__((unused)) bool ConfirmBackward, __attribute__((unused)) bool ConfirmStop, __attribute__((unused)) float RPM) : ACMotorCommonTemplate(SequenceID) {}
   ACMotorStates ACMotorState = ACMotorStates::IDLE;
   bool ConfirmForward = false; //Feedback to the Main module that the command was processed and it can turn off the request flag.
   bool ConfirmBackward = false;
   bool ConfirmStop = false;
   float RPM = 0.0; /// RPM reading from the motor
};