/**@file*/
///< GrowBoxGuy - http:///< sites.google.com/site/growboxguy/
///< Structs for wireless communication - Both the Transmitter and the Receiver needs to know these
///< Defines the package stucture of each message exchanged between the Main and the Hempy module
///< Max 32 bytes can be sent in a single package

#pragma once

#include "TimeLib.h" // Keeping track of time

enum HempyMessages
{
   HempyModuleCommand1,
   HempyModuleResponse1,
   HempyBucketCommand1,
   HempyBucketResponse1,
   HempyBucketCommand2,
   HempyBucketResponse2,
   HempyReset
}; ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824

static const __FlashStringHelper *toText_hempySequenceID(uint8_t SequenceID)
{
   switch (SequenceID)
   {
   case HempyMessages::HempyModuleCommand1:
      return F("HempyModuleCommand1");
      break;
   case HempyMessages::HempyModuleResponse1:
      return F("HempyModuleResponse1");
      break;
   case HempyMessages::HempyBucketCommand1:
      return F("HempyBucketCommand1");
      break;
   case HempyMessages::HempyBucketResponse1:
      return F("HempyBucketResponse1");
      break;
   case HempyMessages::HempyBucketCommand2:
      return F("HempyBucketCommand2");
      break;
   case HempyMessages::HempyBucketResponse2:
      return F("HempyBucketResponse2");
      break;
   case HempyMessages::HempyReset:
      return F("HempyReset");
      break;
   default:
      return F("UNKNOWN");
      break;
   }
}
///< HempyReset should always be the last element
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
   HempyModuleCommand(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   HempyModuleCommand(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) time_t Time, __attribute__((unused)) bool Debug, __attribute__((unused)) bool Metric) : HempyCommonTemplate(SequenceID) {}
   time_t Time = 0;
   bool Debug = true;
   bool Metric = true;
};

struct HempyModuleResponse : HempyCommonTemplate  ///< Hempy module wireless response
{
   HempyModuleResponse(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   HempyModuleResponse(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) bool Status) : HempyCommonTemplate(SequenceID) {}
   bool Status = true;
};

struct HempyBucketCommand : HempyCommonTemplate ///< Hempy bucket wireless command
{
   HempyBucketCommand(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   HempyBucketCommand(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) bool Disable, __attribute__((unused)) bool StartWatering, __attribute__((unused)) bool StopWatering,  __attribute__((unused)) bool TareWeightB,  __attribute__((unused)) bool TareWeightWR, __attribute__((unused)) uint8_t PumpSpeed, __attribute__((unused)) uint16_t PumpTimeOut, __attribute__((unused)) float EvaporationTarget, __attribute__((unused)) float OverflowTarget, __attribute__((unused)) float WasteLimit,  __attribute__((unused)) uint16_t DrainWaitTime, __attribute__((unused)) uint16_t WateringTimeOut) : HempyCommonTemplate(SequenceID) {}
   bool Disable = false;
   bool StartWatering = false;
   bool StopWatering = false;
   bool TareWeightB = false;  //Tare bucket weight scale
   bool TareWeightWR = false; //Tare waste reservoir weight scale
   uint8_t PumpSpeed = 0;
   uint16_t PumpTimeOut = 0;
   float EvaporationTarget = 0.0;
   float OverflowTarget = 0.0;
   float WasteLimit = 0.0;
   uint16_t DrainWaitTime;
   uint16_t WateringTimeOut;
};

struct HempyBucketResponse : HempyCommonTemplate ///< Hempy bucket wireless response
{
   HempyBucketResponse(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   HempyBucketResponse(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) HempyStates HempyState, __attribute__((unused)) WaterPumpStates PumpState, __attribute__((unused)) float WeightB, __attribute__((unused)) float WeightWR, __attribute__((unused)) float NextWatering) : HempyCommonTemplate(SequenceID) {}
   HempyStates HempyState = HempyStates::IDLE;
   WaterPumpStates PumpState = WaterPumpStates::DISABLED;
   float WeightB = 0.0;
   float WeightWR = 0.0;
   float NextWatering = 0.0;
};