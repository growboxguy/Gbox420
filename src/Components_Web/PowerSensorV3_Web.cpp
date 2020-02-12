#include "PowerSensorV3_Web.h"

PowerSensorV3_Web::PowerSensorV3_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort) : PowerSensorV3(Name,Parent,SerialPort), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
}

void PowerSensorV3_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getComponentName(F("P")), getPowerText(true));
    WebServer.setArgString(getComponentName(F("E")), getEnergyText(true));
    WebServer.setArgString(getComponentName(F("V")), getVoltageText(true));
    WebServer.setArgString(getComponentName(F("C")), getCurrentText(true));
    WebServer.setArgString(getComponentName(F("F")), getFrequencyText(true));
    WebServer.setArgString(getComponentName(F("PF")), getPowerFactorText());
  }
}