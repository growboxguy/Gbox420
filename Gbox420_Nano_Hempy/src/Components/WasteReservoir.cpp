#include "WasteReservoir.h"

WasteReservoir::WasteReservoir(const __FlashStringHelper *Name, Module *Parent, Settings::WasteReservoirSettings *DefaultSettings, WeightSensor *WasteWeightSensor) : Common(Name)
{
  this->Parent = Parent;
  this->WasteWeightSensor = WasteWeightSensor;
  WasteLimit = &DefaultSettings->WasteLimit;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("WasteReservoir ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void WasteReservoir::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat);              //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat_P(LongMessage, (PGM_P)F("\"R\":\"")); //< Reserved
  strcat(LongMessage, getReservedText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"F\":\"")); //< Full - Weight limit reached
  strcat(LongMessage, getFullText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"L\":\"")); //Limit
  strcat(LongMessage, getWasteLimitText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void WasteReservoir::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  checkFull();
}

void WasteReservoir::checkFull()
{
  if (WasteWeightSensor->getWeight() > *WasteLimit)
    Full = true;
  else
    Full = false;
}

bool WasteReservoir::checkTarget(float OverflowTarget)
{
  if (Reserved)
  {
    if (WasteWeightSensor->readWeight(false) - StartWeight >= OverflowTarget)
    {
      return true;
    }
  }
  return false;
}

/**
* @brief Reserves the waste reservoir and store the start weight if the reservoir. Returns false if a reservation cannot be made
*/
bool WasteReservoir::setReserved()
{
  if (Reserved || Full)
  {
    return false; ///< Deny a new reservation when already reserved or full
  }
  else
  {
    Reserved = true;
    StartWeight = WasteWeightSensor->getWeight();
    if (*Debug)
    {
      logToSerials(getName(F("reserved")), true, 3);
    }
    return true; ///< Accept reservation
  }
}

void WasteReservoir::clearReservation()
{
  Reserved = false;
  if (*Debug)
  {
    logToSerials(getName(F("released")), true, 3);
  }
}

bool WasteReservoir::getReserved()
{
  return Reserved;
}

char *WasteReservoir::getReservedText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_yesNo(Reserved);
  }
  else
  {
    return toText(Reserved);
  }
}

bool WasteReservoir::getFull()
{
  return Full;
}

char *WasteReservoir::getFullText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_yesNo(Full);
  }
  else
  {
    return toText(Full);
  }
}

void WasteReservoir::setWasteLimit(float Weight)
{
  if (*WasteLimit != Weight)
  {
    *WasteLimit = Weight;
    Parent->getSoundObject()->playOnSound();
  }
}

char *WasteReservoir::getWasteLimitText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_weight(*WasteLimit);
  }
  else
  {
    return toText(*WasteLimit);
  }
}

float WasteReservoir::getWeightIncrease()
{
  return WasteWeightSensor->getWeight(false) - StartWeight;
}