#include "AirPump.h"
#include "Sound.h"

///< AirPump controller (2 speed)

AirPump::AirPump(const __FlashStringHelper *Name,
                 Module *Parent,
                 Settings::AirPumpSettings *DefaultSettings) ///< Original constructor
    : Common(Name),
      Parent(Parent),                ///< Initialize Parent in the constructor
      State(DefaultSettings->State), ///< Initialize reference to DefaultSettings->State
      Pin(DefaultSettings->Pin)      ///< Initialize reference to DefaultSettings->Pin
{
  pinMode(Pin, OUTPUT);
  digitalWrite(Pin, HIGH);
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("AirPump ready"), true, 3);
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void AirPump::report(bool FriendlyFormat)
{
  Common::report(true); ///< Appends "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, getStateText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void AirPump::refresh_Minute()
{
  Common::refresh_Minute();
  checkStatus();
}

void AirPump::checkStatus()
{
  if (State) ///< True turns relay ON (LOW signal activates the Relay)
    digitalWrite(Pin, LOW);
  else
    digitalWrite(Pin, HIGH);
}

void AirPump::TurnOff()
{
  setState(false);
  checkStatus();
}

void AirPump::TurnOn()
{
  setState(true);
  checkStatus();
}

void AirPump::setState(bool NewState)
{
  State = NewState;
  checkStatus();
  Parent->getSoundObject()->playOnOffSound(State);
  Parent->addToLog(getName(getStateText(true)));
}

bool AirPump::getState()
{
  return State;
}

char *AirPump::getStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_onOff(State);
  }
  else
  {
    return toText(State);
  }
}