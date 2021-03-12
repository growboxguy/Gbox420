#include "PowerSensor_Web.h"

PowerSensor_Web::PowerSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort) : Common(Name), PowerSensor(Name, Parent, SerialPort), Common(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToWebsiteQueue_Refresh(this);
}



void PowerSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {

  }
}