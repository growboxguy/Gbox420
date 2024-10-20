#include "TDSSensor_Web.h"

TDSSensor_Web::TDSSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::TDSSensorSettings *DefaultSettings, WaterTempSensor *WaterTempSensor1) : Common(Name), TDSSensor(Name, Parent, DefaultSettings, WaterTempSensor1), Common_Web(Name)
{ ///< constructor
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
}

void TDSSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("R"), true), getTDSText(false)); ///< Update the TDS reading on the web interface (Name_R)
}