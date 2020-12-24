#include "TDSSensor_Web.h"

TDSSensor_Web::TDSSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::TDSSensorSettings *DefaultSettings, WaterTempSensor *WaterTempSensor1) : Common(Name), TDSSensor(Name, Parent, DefaultSettings, WaterTempSensor1), Common_Web(Name)
{ ///< constructor
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToWebsiteQueue_Refresh(this);
}

void TDSSensor_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"TDS\":\""));
  strcat(LongMessage, getTDSText());
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void TDSSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("R")), getTDSText(false));  ///< Update the TDS reading on the web interface (Name_R)
  }
}