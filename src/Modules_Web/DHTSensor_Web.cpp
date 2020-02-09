#include "DHTSensor_Web.h"
#include "GrowBox.h"

DHTSensor_Web::DHTSensor_Web(const __FlashStringHelper *Name, GrowBox *GBox, Settings::DHTSensor_WebSettings *DefaultSettings) : DHTSensor(Name,DefaultSettings)
{
  this->GBox = GBox;
  Sensor = new DHT(*(&DefaultSettings->Pin), *(&DefaultSettings->Type));
  GBox->AddToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  GBox->AddToRefreshQueue_Minute(this);  //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  GBox->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
}

void DHTSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{ //When the website is refreshing
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Temp")), getTempText(true, false)); //Shows the latest reading
    WebServer.setArgString(getWebsiteComponentName(F("Humidity")), getHumidityText(true, false));
  }
}
