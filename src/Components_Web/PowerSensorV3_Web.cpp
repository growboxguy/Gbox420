#include "PowerSensorV3_Web.h"

PowerSensorV3_Web::PowerSensorV3_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort) : Common(Name), Common_Web(Name), PowerSensorV3(Name, Parent, SerialPort)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
}

void PowerSensorV3_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("P"), true), getPowerText(true));
  WebServer.setArgString(getName(F("E"), true), getEnergyText(true));
  WebServer.setArgString(getName(F("V"), true), getVoltageText(true));
  WebServer.setArgString(getName(F("C"), true), getCurrentText(true));
  WebServer.setArgString(getName(F("F"), true), getFrequencyText(true));
  WebServer.setArgString(getName(F("PF"), true), getPowerFactorText());
}