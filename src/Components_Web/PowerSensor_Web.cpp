#include "PowerSensor_Web.h"

PowerSensor_Web::PowerSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort) : PowerSensor(Name,Parent,SerialPort), Common(Name), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_FiveSec(this); //Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  Parent->addToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
}

void PowerSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("P")), getPowerText(true));
    WebServer.setArgString(getComponentName(F("E")), getEnergyText(true));
    WebServer.setArgString(getComponentName(F("V")), getVoltageText(true));
    WebServer.setArgString(getComponentName(F("C")), getCurrentText(true));
  }
}