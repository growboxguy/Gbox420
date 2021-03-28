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
      return F("HMC1");
      break;
   case HempyMessages::HempyModuleResponse1:
      return F("HMR1");
      break;
   case HempyMessages::HempyBucketCommand1:
      return F("HC1");
      break;
   case HempyMessages::HempyBucketResponse1:
      return F("HR1");
      break;
   case HempyMessages::HempyBucketCommand2:
      return F("HC2");
      break;
   case HempyMessages::HempyBucketResponse2:
      return F("HR2");
      break;
   case HempyMessages::HempyReset:
      return F("HR");
      break;
   default:
      return F("?");
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
   HempyModuleCommand(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) time_t Time, __attribute__((unused)) bool Debug, __attribute__((unused)) bool Metric, __attribute__((unused)) uint16_t SerialReportFrequency, __attribute__((unused)) bool SerialReportDate, __attribute__((unused)) bool SerialReportMemory, __attribute__((unused)) bool SerialReportToText, __attribute__((unused)) bool SerialReportToJSON, __attribute__((unused)) bool SerialReportWireless) : HempyCommonTemplate(SequenceID) {}
   time_t Time = 0;
   bool Debug = true;
   bool Metric = true;
   uint16_t SerialReportFrequency = 0;
   bool SerialReportDate = true;
   bool SerialReportMemory = true;
   bool SerialReportToText = true;
   bool SerialReportToJSON = true;
   bool SerialReportWireless = true;
};

struct HempyModuleResponse : HempyCommonTemplate ///< Hempy module wireless response
{
   HempyModuleResponse(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   HempyModuleResponse(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) bool Status) : HempyCommonTemplate(SequenceID) {}
   bool Status = true;
};

struct HempyBucketCommand : HempyCommonTemplate ///< Hempy bucket wireless command
{
   HempyBucketCommand(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   HempyBucketCommand(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) bool Disable, __attribute__((unused)) bool StartWatering, __attribute__((unused)) bool StopWatering, __attribute__((unused)) bool TareWeightB, __attribute__((unused)) bool TareWeightDW, __attribute__((unused)) bool TareWeightWR, __attribute__((unused)) uint8_t PumpSpeed, __attribute__((unused)) uint16_t PumpTimeOut, __attribute__((unused)) float DryWeight, __attribute__((unused)) float EvaporationTarget, __attribute__((unused)) float OverflowTarget, __attribute__((unused)) float WasteLimit, __attribute__((unused)) uint16_t DrainWaitTime) : HempyCommonTemplate(SequenceID) {}
   bool Disable = false;
   bool StartWatering = false;
   bool StopWatering = false;
   bool TareWeightB = false;  //Tare bucket weight scale
   bool TareWeightDW = false; //Tare dry/wet weight
   bool TareWeightWR = false; //Tare waste reservoir weight scale
   uint8_t PumpSpeed = 0;
   uint16_t PumpTimeOut = 0;
   float DryWeight = NAN;
   float EvaporationTarget = 0.0;
   float OverflowTarget = 0.0;
   float WasteLimit = 0.0;
   uint16_t DrainWaitTime;
};

struct HempyBucketResponse : HempyCommonTemplate ///< Hempy bucket wireless response
{
   HempyBucketResponse(__attribute__((unused)) HempyMessages SequenceID) : HempyCommonTemplate(SequenceID) {}
   HempyBucketResponse(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) HempyStates HempyState, __attribute__((unused)) WaterPumpStates PumpState, __attribute__((unused)) float WeightB, __attribute__((unused)) float WeightWR, __attribute__((unused)) float DryWeight, __attribute__((unused)) float WetWeight) : HempyCommonTemplate(SequenceID) {}
   HempyStates HempyState = HempyStates::IDLE;
   WaterPumpStates PumpState = WaterPumpStates::DISABLED;
   float WeightB = 0.0;
   float WeightWR = 0.0;
   float DryWeight = 0.0;
   float WetWeight = 0.0;
};