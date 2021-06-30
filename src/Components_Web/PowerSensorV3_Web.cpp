#include "PowerSensorV3_Web.h"

PowerSensorV3_Web::PowerSensorV3_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort) : Common(Name), PowerSensorV3(Name, Parent, SerialPort), Common_Web(Name)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
}

void PowerSensorV3_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  WebServer.setArgString(getName(F("P")), getPowerText(true));
  WebServer.setArgString(getName(F("E")), getEnergyText(true));
  WebServer.setArgString(getName(F("V")), getVoltageText(true));
  WebServer.setArgString(getName(F("C")), getCurrentText(true));
  WebServer.setArgString(getName(F("F")), getFrequencyText(true));
  WebServer.setArgString(getName(F("PF")), getPowerFactorText());
}