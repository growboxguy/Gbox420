#include "HempyBucket_Web.h"

HempyBucket_Web::HempyBucket_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WasteReservoir *BucketWasteReservoir, WaterPump *BucketPump) : Common(Name), Common_Web(Name), HempyBucket(Name, Parent, DefaultSettings, BucketWeightSensor, BucketWasteReservoir, BucketPump), Parent(Parent)
{
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void HempyBucket_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("ET"), true), getEvaporationTargetText());
  WebServer.setArgString(getName(F("OF"), true), getDrainTargetWeightText());
  WebServer.setArgInt(getName(F("D"), true), getDrainWaitTime());
  WebServer.setArgString(getName(F("DW"), true), getDryWeightText());
  //WebServer.setArgInt(getName(F("PS"),true), HempyBucket1Command1ToSend.PumpSpeed);
  //WebServer.setArgInt(getName(F("T"),true), HempyBucket1Command1ToSend.PumpTimeOut);
}

void HempyBucket_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  //WebServer.setArgFloat(getName(F("W"),true), BucketWeightSensor->getWeightText());
  //WebServer.setArgString(getName(F("W"),true), toText_weight(HempyBucket1Response1Received.WeightB));
  WebServer.setArgString(getName(F("DWW"), true), toText(getDryWeight(), "/", getWetWeight()));
  WebServer.setArgString(getName(F("S"), true), getStateText(true));
  //WebServer.setArgString(getName(F("P"),true), toText_waterPumpState(HempyBucket1Response1Received.PumpState));
  //WebServer.setArgString(getName(F("OS"),true), toText(*BucketWeightSensor->Offset));
  //WebServer.setArgString(getName(F("SC"),true), toText(*BucketWeightSensor->Scale));
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/

bool HempyBucket_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("On")) == 0)
    {
      startWateringRequest();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Off")) == 0)
    {
      stopWateringRequest();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Dis")) == 0)
    {
      disableRequest();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("TareDW")) == 0)
    {
      tareDryWetWeight();
      Parent->addToLog(F("Taring Dry/Wet"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("ET")) == 0)
    {
      setEvaporationTarget(toFloat(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OF")) == 0)
    {
      setDrainTargetWeight(toFloat(Data));
      Parent->addToLog(F("Targets updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("D")) == 0)
    {
      setDrainWaitTime(toInt(Data));
      Parent->addToLog(F("Drain wait updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SW")) == 0)
    {
      setStartWeight(toFloat(Data));
      Parent->addToLog(F("Start weight updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("WI")) == 0)
    {
      setWateringIncrement(toFloat(Data));
      Parent->addToLog(F("Watering increment updated"), false);
    }
   
    return true; //Match found
  }
}