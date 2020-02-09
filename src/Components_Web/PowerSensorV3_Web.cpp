#include "PowerSensorV3_Web.h"

PowerSensorV3_Web::PowerSensorV3_Web(const __FlashStringHelper *Name, GrowBox *GBox, HardwareSerial *SerialPort) : PowerSensorV3(Name,Parent,SerialPort)
{
  Parent->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
}

void PowerSensorV3_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Power")), getPowerText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Energy")), getEnergyText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Voltage")), getVoltageText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Current")), getCurrentText(true));
    WebServer.setArgString(getWebsiteComponentName(F("Frequency")), getFrequencyText(true));
    WebServer.setArgString(getWebsiteComponentName(F("PowerFactor")), getPowerFactorText());
  }
}