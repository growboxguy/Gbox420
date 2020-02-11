#include "PowerSensor_Web.h"

PowerSensor_Web::PowerSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort) : PowerSensor(Name,Parent,SerialPort)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
}

void PowerSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getComponentName(F("Power")), getPowerText(true));
    WebServer.setArgString(getComponentName(F("Energy")), getEnergyText(true));
    WebServer.setArgString(getComponentName(F("Voltage")), getVoltageText(true));
    WebServer.setArgString(getComponentName(F("Current")), getCurrentText(true));
  }
}