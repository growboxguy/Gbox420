#include "PowerSensor_Web.h"

PowerSensor_Web::PowerSensor_Web(const __FlashStringHelper *Name, Modul *Parent, HardwareSerial *SerialPort) : PowerSensor(Name,Parent,SerialPort)
{
  Parent->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
}

void PowerSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Power")), getPowerText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Energy")), getEnergyText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Voltage")), getVoltageText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Current")), getCurrentText(true));
  }
}