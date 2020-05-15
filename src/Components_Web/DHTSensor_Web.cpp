#include "DHTSensor_Web.h"

DHTSensor_Web::DHTSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DHTSensorSettings *DefaultSettings) : Common(Name), DHTSensor(Name,Parent,DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Sensor = new DHT(*(&DefaultSettings->Pin), *(&DefaultSettings->Type)); 
  Parent->addToWebsiteQueue_Refresh(this); 
  Parent->addToReportQueue(this);          
  Parent->addToRefreshQueue_FiveSec(this);  
}

void DHTSensor_Web::reportToJSON()
{
    Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"Temp\":\""));
    strcat(LongMessage, getTempText());
    strcat_P(LongMessage, (PGM_P)F("\",\"Humidity\":\""));
    strcat(LongMessage, getHumidityText());   
    strcat_P(LongMessage, (PGM_P)F("\"}"));  ///< closing the curly bracket
}

void DHTSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{ ///When the website is refreshing
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("T")), getTempText(true)); ///Shows the latest reading
    WebServer.setArgString(getComponentName(F("H")), getHumidityText(true));
  }
}
