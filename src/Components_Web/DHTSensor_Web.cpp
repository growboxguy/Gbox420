#include "DHTSensor_Web.h"

DHTSensor_Web::DHTSensor_Web(const __FlashStringHelper *Name, Module *Parent, Settings::DHTSensorSettings *DefaultSettings) : DHTSensor(Name,Parent,DefaultSettings)
{
  this->Parent = Parent;
  Sensor = new DHT(*(&DefaultSettings->Pin), *(&DefaultSettings->Type)); 
  Parent->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
}

void DHTSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{ //When the website is refreshing
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Temp")), getTempText(true, false)); //Shows the latest reading
    WebServer.setArgString(getWebsiteComponentName(F("Humidity")), getHumidityText(true, false));
  }
}
