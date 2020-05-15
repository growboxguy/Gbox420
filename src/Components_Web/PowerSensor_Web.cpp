#include "PowerSensor_Web.h"

PowerSensor_Web::PowerSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort) : Common(Name), PowerSensor(Name,Parent,SerialPort), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);          
  Parent->addToRefreshQueue_FiveSec(this); 
  Parent->addToWebsiteQueue_Refresh(this); 
}

void PowerSensor_Web::reportToJSON()
{
    Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end 

    strcat_P(LongMessage, (PGM_P)F("\"Power\":\""));
    strcat(LongMessage, getPowerText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"Energy\":\""));
    strcat(LongMessage, getEnergyText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"Voltage\":\""));
    strcat(LongMessage, getVoltageText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"Current\":\""));
    strcat(LongMessage, getCurrentText(false));
    strcat_P(LongMessage, (PGM_P)F("\"}"));  ///< closing the curly bracket
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