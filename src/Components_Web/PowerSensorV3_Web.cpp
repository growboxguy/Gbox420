#include "PowerSensorV3_Web.h"

PowerSensorV3_Web::PowerSensorV3_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort) : Common(Name), PowerSensorV3(Name, Parent, SerialPort), Common(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToWebsiteQueue_Refresh(this);
}

void PowerSensorV3_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("P")), getPowerText(true));
    WebServer.setArgString(getComponentName(F("E")), getEnergyText(true));
    WebServer.setArgString(getComponentName(F("V")), getVoltageText(true));
    WebServer.setArgString(getComponentName(F("C")), getCurrentText(true));
    WebServer.setArgString(getComponentName(F("F")), getFrequencyText(true));
    WebServer.setArgString(getComponentName(F("PF")), getPowerFactorText());
  }
}