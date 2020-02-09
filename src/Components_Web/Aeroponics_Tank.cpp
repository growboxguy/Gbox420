#include "Aeroponics_Tank.h"
#include "GrowBox.h"

Aeroponics_Tank::Aeroponics_Tank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor) : Aeroponics(&(*Name), &(*GBox), &(*DefaultSettings), &(*FeedbackPressureSensor))
{ //constructor
  SpraySolenoidPin = &TankSpecificSettings->SpraySolenoidPin;
  PressureLow = &TankSpecificSettings->PressureLow;   //Aeroponics - Turn on pump below this pressure (bar)
  PressureHigh = &TankSpecificSettings->PressureHigh; //Aeroponics - Turn off pump above this pressure (bar)
  pinMode(*SpraySolenoidPin, OUTPUT);
  digitalWrite(*SpraySolenoidPin, HIGH); //initialize off
  logToSerials(F("Aeroponics_Tank object created"), true, 1);
  sprayNow(false); //This is a safety feature,start with a spray after a reset
}

void Aeroponics_Tank::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgFloat(getWebsiteComponentName(F("PresLow")), *PressureLow);
    WebServer.setArgFloat(getWebsiteComponentName(F("PresHigh")), *PressureHigh);
  }
  Aeroponics::websiteEvent_Load(url);
}

void Aeroponics_Tank::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Refill")) == 0)
    {
      refillTank();
    }
    else
      Aeroponics::websiteEvent_Button(Button);
  }
}

void Aeroponics_Tank::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("PresLow")) == 0)
    {
      setPressureLow(WebServer.getArgFloat());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PresHigh")) == 0)
    {
      setPressureHigh(WebServer.getArgFloat());
    }
    else
      Aeroponics::websiteEvent_Field(Field);
  }
}

void Aeroponics_Tank::refresh_Sec()
{
  if (*DebugEnabled)
    Common_Web::refresh_Sec();
  if (PumpOn)
  { //if pump is on
    if (Aeroponics::FeedbackPressureSensor->getPressure() >= *PressureHigh)
    { //refill complete, target pressure reached
      setPumpOff(false);
      logToSerials(F("Pressure tank recharged"), true);
    }
    else
    {
      if (millis() - PumpTimer >= ((uint32_t)*PumpTimeout * 60000))
      {                    //If pump timeout reached
        setPumpOff(false); //turn of pump,
        if (!MixInProgress)
        {                //if mixing was not active and refill did not finish within timeout= pump must be broken
          PumpDisable(); //automatically disable pump if it is suspected to be broken
          //   sendEmailAlert(F("Aeroponics%20pump%20failed"));
        }
        logToSerials(F("Pump timeout reached"), true);
      }
      if (!MixInProgress && BypassSolenoidOn && millis() - PumpTimer >= ((uint32_t)*PrimingTime * 1000))
      { //self priming timeout reached, time to start refilling
        if (*DebugEnabled)
          logToSerials(F("Starting refill"), true);
        BypassSolenoidOn = false;
        PumpTimer = millis(); //reset timer to start measuring refill time
      }
    }
  }
  else
  {                           //pump is off
    BypassSolenoidOn = false; //Should not leave the solenoid turned on
  }
  if (PumpOK && Aeroponics::FeedbackPressureSensor->getPressure() <= *PressureLow)
  { //if pump is not disabled and Pressure reached low limit: turn on pump
    if (!PumpOn && !BypassSolenoidOn)
    { //start the bypass
      if (*DebugEnabled)
        logToSerials(F("Starting bypass"), true);
      BypassSolenoidOn = true;
      PumpOn = true;
      PumpTimer = millis();
    }
  }
  if (SpraySolenoidOn)
  { //if spray is on
    if (millis() - SprayTimer >= ((uint32_t)*Duration * 1000))
    { //if time to stop spraying (Duration in Seconds)
      SpraySolenoidOn = false;
      logToSerials(F("Stopping spray"), true);
      Parent->Sound1->playOffSound();
      SprayTimer = millis();
    }
  }
  else
  { //if spray is off
    if (*SprayEnabled && millis() - SprayTimer >= ((uint32_t)*Interval * 60000))
    { //if time to start spraying (AeroInterval in Minutes)
      SpraySolenoidOn = true;
      Parent->Sound1->playOnSound();
      if (*DebugEnabled)
        logToSerials(F("Starting spray"), true);
      SprayTimer = millis();
    }
  }
  checkRelays();
}

void Aeroponics_Tank::report()
{
  Common_Web::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("Pressure:"));
  strcat(LongMessage, FeedbackPressureSensor->getPressureText(true, false));
  logToSerials(&LongMessage, false, 1); //first print Aeroponics_Tank specific report, without a line break
  Aeroponics::report();                 //then print parent class report
}

void Aeroponics_Tank::checkRelays()
{
  if (SpraySolenoidOn)
    digitalWrite(*SpraySolenoidPin, LOW);
  else
    digitalWrite(*SpraySolenoidPin, HIGH);
  Aeroponics::checkRelays();
}

void Aeroponics_Tank::setPressureLow(float PressureLow)
{
  *(this->PressureLow) = PressureLow;
}

void Aeroponics_Tank::setPressureHigh(float PressureHigh)
{
  *(this->PressureHigh) = PressureHigh;
  Parent->addToLog(F("Tank limits updated"));
}

void Aeroponics_Tank::sprayNow(bool FromWebsite)
{
  if (*SprayEnabled || FromWebsite)
  {
    MixInProgress = false;
    SprayTimer = millis();
    SpraySolenoidOn = true;
    checkRelays();
    Parent->Sound1->playOnSound();
    if (FromWebsite)
      Parent->addToLog(F("Aeroponics spraying"));
    else
      logToSerials(F("Aeroponics spraying"), true, 3);
  }
}

void Aeroponics_Tank::sprayOff()
{
  SpraySolenoidOn = false;
  checkRelays();
  Parent->addToLog(F("Aeroponics spray OFF"));
}

char *Aeroponics_Tank::sprayStateToText()
{
  if (!*SprayEnabled && !SpraySolenoidOn)
    return (char *)"DISABLED";
  else
  {
    if (SpraySolenoidOn)
      return (char *)"ON";
    else
      return (char *)"ENABLED";
  }
}

void Aeroponics_Tank::refillTank()
{
  Parent->addToLog(F("Refilling tank"));
  PumpOK = true;
  BypassSolenoidOn = true;
  PumpOn = true;
  PumpTimer = millis();
  checkRelays();
}