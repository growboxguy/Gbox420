#include "PowerSensor_Web.h"

PowerSensor_Web::PowerSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort) : Common(Name), PowerSensor(Name, Parent, SerialPort), Common_Web(Name), Parent(Parent)
{
  Parent->addToWebsiteQueue_Refresh(this);
}

void PowerSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("P")), getPowerText(true));
  WebServer.setArgString(getName(F("E")), getEnergyText(true));
  WebServer.setArgString(getName(F("V")), getVoltageText(true));
  WebServer.setArgString(getName(F("C")), getCurrentText(true));
}