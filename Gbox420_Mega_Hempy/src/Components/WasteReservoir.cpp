#include "WasteReservoir.h"

WasteReservoir::WasteReservoir(const __FlashStringHelper *Name, Module *Parent, Settings::WasteReservoirSettings *DefaultSettings, WeightSensor *WasteWeightSensor)
    : Common(Name),
      Parent(Parent),
      WasteWeightSensor(WasteWeightSensor),
      WasteLimit(DefaultSettings->WasteLimit)
{
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
  strcat_P(LongMessage, (PGM_P)F("\"S\":\"")); //< Reserved
  strcat(LongMessage, getStateText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"L\":\"")); // Limit
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
    updateState(WasteReservoirStates::FULL);
  else if (State != WasteReservoirStates::RESERVED)
    updateState(WasteReservoirStates::IDLE);
}

bool WasteReservoir::checkTarget(float DrainTargetWeight)
{
  if (WasteWeightSensor->readWeight() - StartWeight >= DrainTargetWeight || State == WasteReservoirStates::FULL) ///< When the overflow target is reached or the reservoir is full: Signal back the target is reached
  {
    return true;
  }
  return false;
}

void WasteReservoir::updateState(WasteReservoirStates NewState)
{
  if (State != NewState)
  {
    StateTimer = millis();                         ///< Start measuring the time spent in the new State
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat(LongMessage, getName(F("state: ")));
    strcat(LongMessage, toText_wasteReservoirStates(State));
    strcat_P(LongMessage, (PGM_P)F(" -> "));
    strcat(LongMessage, toText_wasteReservoirStates(NewState));
    logToSerials(&LongMessage, true, 3);
    State = NewState;
  }
}

/**
 * @brief Reserves the waste reservoir and store the start weight if the reservoir. Returns false if a reservation cannot be made
 */
bool WasteReservoir::setReserved()
{
  if (State == WasteReservoirStates::RESERVED || State == WasteReservoirStates::FULL)
  {
    return false; ///< Deny a new reservation when already reserved or full
  }
  else
  {
    updateState(WasteReservoirStates::RESERVED);
    StartWeight = WasteWeightSensor->getWeight();
    return true; ///< Accept reservation
  }
}

void WasteReservoir::clearReservation()
{
  if (State == WasteReservoirStates::RESERVED)
    updateState(WasteReservoirStates::IDLE);
}

WasteReservoirStates WasteReservoir::getState()
{
  return State;
}

char *WasteReservoir::getStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_wasteReservoirStates(State);
  }
  else
  {
    return toText((int)State);
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
  return WasteWeightSensor->getWeight() - StartWeight;
}