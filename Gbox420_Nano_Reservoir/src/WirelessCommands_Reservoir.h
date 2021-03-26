/**@file*/
///< GrowBoxGuy - http:///< sites.google.com/site/growboxguy/
///< Structs for wireless communication - Both the Transmitter and the Receiver needs to know these
///< Defines the package stucture of each message exchanged between the Main and the Reservoir module
///< Max 32 bytes can be sent in a single package

#pragma once

#include "TimeLib.h" // Keeping track of time

enum ReservoirMessages
{
   ReservoirModuleCommand1,
   ReservoirModuleResponse1,
   ReservoirCommand1,
   ReservoirResponse1,
   ReservoirReset
}; ///< ReservoirReset should always be the last element. An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824

static const __FlashStringHelper *toText_reservoirSequenceID(uint8_t SequenceID)
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
   ReservoirModuleCommand(__attribute__((unused)) ReservoirMessages SequenceID) : ReservoirCommonTemplate(SequenceID) {}
   ReservoirModuleCommand(__attribute__((unused)) ReservoirMessages SequenceID, __attribute__((unused)) time_t Time, __attribute__((unused)) bool Debug, __attribute__((unused)) bool Metric, __attribute__((unused)) bool ReportDate, __attribute__((unused)) bool ReportMemory, __attribute__((unused)) bool ReportToText, __attribute__((unused)) bool ReportToJSON) : ReservoirCommonTemplate(SequenceID) {}
   time_t Time = 0;
   bool Debug = true;
   bool Metric = true;
   bool ReportDate = true;
   bool ReportMemory = true;
   bool ReportToText = true;
   bool ReportToJSON = true;
};

struct ReservoirModuleResponse : ReservoirCommonTemplate ///< Max 32 bytes. Response sent back to the Main module
{
   ReservoirModuleResponse(__attribute__((unused)) ReservoirMessages SequenceID) : ReservoirCommonTemplate(SequenceID) {}
   ReservoirModuleResponse(__attribute__((unused)) ReservoirMessages SequenceID, __attribute__((unused)) bool Status) : ReservoirCommonTemplate(SequenceID) {}
   bool Status = true;
};

struct ReservoirCommand : ReservoirCommonTemplate ///< Max 32 bytes. Command sent by the Main module
{
   ReservoirCommand(__attribute__((unused)) ReservoirMessages SequenceID) : ReservoirCommonTemplate(SequenceID) {}
   ReservoirCommand(__attribute__((unused)) ReservoirMessages SequenceID, __attribute__((unused)) bool TareWeight) : ReservoirCommonTemplate(SequenceID) {}
   bool TareWeight = false;
};

struct ReservoirResponse : ReservoirCommonTemplate ///< Max 32 bytes. Response sent back to the Main module
{
   ReservoirResponse(__attribute__((unused)) ReservoirMessages SequenceID) : ReservoirCommonTemplate(SequenceID) {}
   ReservoirResponse(__attribute__((unused)) ReservoirMessages SequenceID, __attribute__((unused)) float PH, __attribute__((unused)) float TDS, __attribute__((unused)) float Weight, __attribute__((unused)) float WaterTemperature, __attribute__((unused)) float AirTemperature, __attribute__((unused)) float Humidity) : ReservoirCommonTemplate(SequenceID) {}
   float PH = 0.0;
   float TDS = 0.0;
   float Weight = 0.0;
   float WaterTemperature = 0.0;
   float AirTemperature = 0.0;
   float Humidity = 0.0;
};