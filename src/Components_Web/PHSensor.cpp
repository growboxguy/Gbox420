#include "PHSensor.h"
#include "GrowBox.h"

PHSensor::PHSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PHSensorSettings *DefaultSettings) : Common_Web(Name)
{ //constructor
  this->Parent = Parent;
  this->Pin = &DefaultSettings->Pin;
  this->Intercept = &DefaultSettings->Intercept;
  this->Slope = &DefaultSettings->Slope;
  pinMode(*Pin, INPUT);
  PH = new RollingAverage();
  Parent->AddToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  Parent->AddToRefreshQueue_Minute(this);  //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  Parent->AddToWebsiteQueue_Load(this);    //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->AddToWebsiteQueue_Field(this);   //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  Parent->AddToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  logToSerials(F("PHSensor object created"), true, 1);
}

void PHSensor::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/Settings.html.json") == 0)
  {
    //WebServer.setArgString(F("PHAlertLow"), toText(GBox -> Reservoir -> PHAlertLow));
    //WebServer.setArgString(F("PHAlertHigh"), toText(GBox -> BoxSettings -> PHAlertHigh));
    WebServer.setArgString(getWebsiteComponentName(F("Slope")), toPrecisionText(*Slope));
    WebServer.setArgString(getWebsiteComponentName(F("Intercept")), toPrecisionText(*Intercept));
  }
}

void PHSensor::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("PH")), getPHText(false));
  }
}

void PHSensor::websiteEvent_Button(char *Button)
{ //When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("ReadRaw")) == 0)
    {
      updatePH(true);
    }
  }
}

void PHSensor::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Slope")) == 0)
    {
      setSlope(WebServer.getArgFloat());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Intercept")) == 0)
    {
      setIntercept(WebServer.getArgFloat());
    }
  }
}

void PHSensor::refresh_Minute()
{
  if (*DebugEnabled)
    Common_Web::refresh_Minute();
  updatePH(false);
}

void PHSensor::report()
{
  Common_Web::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("PH:"));
  strcat(LongMessage, PH->getFloatText(true));
  logToSerials(&LongMessage, true, 1);
}

float PHSensor::getPH(bool ReturnAverage)
{
  return PH->getFloat(ReturnAverage);
}

char *PHSensor::getPHText(bool ReturnAverage)
{
  return PH->getFloatText(ReturnAverage);
}

void PHSensor::updatePH(bool ShowRaw)
{
  int PHRaw = analogRead(*Pin);
  if (ShowRaw)
  {
    strncpy_P(LongMessage, (PGM_P)F("PH analog read: "), MaxTextLength);
    strcat(LongMessage, toText(PHRaw));
    Parent->addToLog(LongMessage);
  }
  PH->updateAverage((*Slope) * PHRaw + (*Intercept));
}

void PHSensor::setSlope(float Value)
{
  *Slope = Value;
  Parent->addToLog(F("PH slope updated"));
}

void PHSensor::setIntercept(float Value)
{
  *Intercept = Value;
  Parent->addToLog(F("PH intercept updated"));
}