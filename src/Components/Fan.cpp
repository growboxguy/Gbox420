#include "Fan.h"
#include "Sound.h"

///< Fan controller (2 speed)

Fan::Fan(const __FlashStringHelper *Name, 
         Module *Parent, 
         Settings::FanSettings *DefaultSettings)  ///< Original constructor
  : Common(Name), 
    Parent(Parent),     ///< Initialize Parent in the constructor
    State(DefaultSettings->State),   ///< Initialize reference to DefaultSettings->State
    HighSpeed(DefaultSettings->HighSpeed), ///< Initialize reference to DefaultSettings->HighSpeed
    OnOffPin(DefaultSettings->OnOffPin),   ///< Initialize reference to DefaultSettings->OnOffPin
    SpeedPin(DefaultSettings->SpeedPin)    ///< Initialize reference to DefaultSettings->SpeedPin
{
  pinMode(OnOffPin, OUTPUT);
  digitalWrite(OnOffPin, HIGH); ///< Turn relay off initially
  pinMode(SpeedPin, OUTPUT);
  digitalWrite(SpeedPin, HIGH); ///< Turn relay off initially
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("Fan ready"), true, 3);
}

void Fan::refresh_Minute()
{
  Common::refresh_Minute();
  checkFanStatus();
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void Fan::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, fanSpeedText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void Fan::checkFanStatus()
{
  if (State) ///< True turns relay ON (LOW signal activates the Relay)
    digitalWrite(OnOffPin, LOW);
  else
    digitalWrite(OnOffPin, HIGH);
  if (HighSpeed)
    digitalWrite(SpeedPin, LOW);
  else
    digitalWrite(SpeedPin, HIGH);
}

void Fan::TurnOff()
{
  State = false;
  HighSpeed = false;
  checkFanStatus();
  Parent->addToLog(getName(fanSpeedText(true)));
  Parent->getSoundObject()->playOffSound();
}

void Fan::SetLowSpeed()
{
  State = true;
  HighSpeed = false;
  checkFanStatus();
  Parent->addToLog(getName(fanSpeedText(true)));
  Parent->getSoundObject()->playOnSound();
}

void Fan::SetHighSpeed()
{
  State = true;
  HighSpeed = true;
  checkFanStatus();
  Parent->addToLog(getName(fanSpeedText(true)));
  Parent->getSoundObject()->playOnSound();
}

uint8_t Fan::fanSpeed()
{
  if (!State)
    return 0;
  else if (HighSpeed)
    return 2;
  else
    return 1;
}

char *Fan::fanSpeedText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    if (!State)
      return (char *)"OFF";
    else if (HighSpeed)
      return (char *)"HIGH";
    else
      return (char *)"LOW";
  }
  else
  {
    if (!State)
      return (char *)"0";
    else if (HighSpeed)
      return (char *)"2";
    else
      return (char *)"1";
  }
}

// WebServer.setArgBoolean(F("AutoIFan"),GBox -> ModuleSettings -> AutomaticIFan);
// WebServer.setArgString(F("IFanSwitchTemp"), toText(GBox -> ModuleSettings -> IFanSwitchTemp));
// WebServer.setArgBoolean(F("AutoEFan"), GBox -> ModuleSettings -> AutomaticEFan);
// WebServer.setArgString(F("EFanHighHumid"), toText(GBox -> ModuleSettings -> EFanHighHumid));
// WebServer.setArgString(F("EFanLowHumid"), toText(GBox -> ModuleSettings -> EFanLowHumid));
// WebServer.setArgString(F("EFanOffHumid"), toText(GBox -> ModuleSettings -> EFanOffHumid));