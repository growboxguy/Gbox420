/**@file*/
///< GrowBoxGuy - https://sites.google.com/site/growboxguy/
///< Structs for wireless communication using an nRF24L01+ transceiver

///< Defines the stucture of each message exchanged between the Main and the Hempy module
///< Both the Transmitter (Main module) and the Receiver (Hempy module) needs to include this
///< Max 32 bytes can be sent in one message

///< The Main module is always the Transmitter, sending a Command defined in HempyMessages
///< The Hempy module is always the Receiver that instantly replies to an incoming message with a pre-cached Acknowledgement(ACK) package
///< HempyReset is a special message:
///<   -  Transmitter (Main module) always starts the multi-message exchange with this message
///<   -  When the Receiver gets this message it pre-loads the ACK message for the first "real" message it will receive from the Main module

#pragma once

#include "TimeLib.h" // Keeping track of time

enum HempyMessages
{
   HempyModuleCommand1, /// First "real" message where the module-level variables are synced: Time, Debug, Serial logging settings
   HempyModuleResponse1,
   HempyBucket1Command1,
   HempyBucket1Response1,
   HempyBucket1Command2,
   HempyBucket1Response2,
   HempyBucket2Command1,
   HempyBucket2Response1,
   HempyBucket2Command2,
   HempyBucket2Response2,
   HempyReset /// Special command sent at the start and end of a multi-message exchange. Should always be the last
}; ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824

__attribute__((unused)) static const __FlashStringHelper *toText_hempySequenceID(uint8_t SequenceID)
{
   switch (SequenceID)
   {
   case HempyMessages::HempyModuleCommand1:
      return F("HMC1");
      break;
   case HempyMessages::HempyModuleResponse1:
      return F("HMR1");
      break;
   case HempyMessages::HempyBucket1Command1:
      return F("H1C1");
      break;
   case HempyMessages::HempyBucket1Response1:
      return F("H1R1");
      break;
   case HempyMessages::HempyBucket1Command2:
      return F("H1C2");
      break;
   case HempyMessages::HempyBucket1Response2:
      return F("H1R2");
      break;
   case HempyMessages::HempyBucket2Command1:
      return F("H2C1");
      break;
   case HempyMessages::HempyBucket2Response1:
      return F("H2R1");
      break;
   case HempyMessages::HempyBucket2Command2:
      return F("H2C2");
      break;
   case HempyMessages::HempyBucket2Response2:
      return F("H2R2");
      break;
   case HempyMessages::HempyReset:
      return F("HR");
      break;
   default:
      return F("?");
      break;
   }
}

struct HempyCommonTemplate ///< Hempy wireless template - Shared between Command and Respone packages
{
   HempyMessages SequenceID; ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data
   HempyCommonTemplate(HempyMessages SequenceID)
   {
      this->SequenceID = SequenceID;
   }
};

struct HempyModuleCommand : HempyCommonTemplate ///< Hempy module wireless command
{
   HempyModuleCommand(HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   time_t Time = 0;
   bool Debug = true;
   bool Metric = true;
   bool SerialReportDate = true;
   bool SerialReportMemory = true;
   bool SerialReportJSONFriendly = true;
   bool SerialReportJSON = true;
   bool SerialReportWireless = true;
};

struct HempyModuleResponse : HempyCommonTemplate ///< Hempy module wireless response
{
   HempyModuleResponse(HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   bool Status = false;
};

struct HempyBucketCommand1 : HempyCommonTemplate ///< Hempy bucket wireless command
{
   HempyBucketCommand1(HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   uint8_t PumpSpeed = 0;
   uint16_t PumpTimeOut = 0;
   uint16_t DrainWaitTime = 0;
   bool Disable = false; ///< Flag to signal a request to disable the watering logic. Flag is kept true until the Receiver confirms processing the request.
   bool StartWatering = false;
   bool StopWatering = false;
   bool TareWeightB = false;  ///< Flag to signal a request to Tare bucket weight scale
   bool TareWeightDW = false; ///< Flag to signal a request to Tare dry/wet weight
};

struct HempyBucketResponse1 : HempyCommonTemplate ///< Hempy bucket wireless response
{
   HempyBucketResponse1(HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   HempyStates HempyState = HempyStates::DISABLED;
   WaterPumpStates PumpState = WaterPumpStates::DISABLED;
   bool ConfirmDisable = false; // Feedback to the Main module that the command was processed and it can turn off the request flag.
   bool ConfirmStartWatering = false;
   bool ConfirmStopWatering = false;
   bool ConfirmTareWeightB = false;
   bool ConfirmTareWeightDW = false;
   float WeightB = 0.0; // Bucket weight
   float DryWeight = 0.0;
   float WetWeight = 0.0;
};

struct HempyBucketCommand2 : HempyCommonTemplate ///< Hempy bucket wireless command - Part2 since did not fit within HempyBucketCommand1 (max 32byte)
{
   HempyBucketCommand2(HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   float StartWeight = 0.0;
   float WateringIncrement = 0.0;
   float EvaporationTarget = 0.0;
   float DrainTargetWeight = 0.0;
   float MaxWeight = 0.0;
};

struct HempyBucketResponse2 : HempyCommonTemplate ///< Hempy bucket wireless response
{
   HempyBucketResponse2(HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   // Nothing to report back since everything fit to HempyBucketResponse1
};
