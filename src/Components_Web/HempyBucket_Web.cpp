#include "HempyBucket_Web.h"

HempyBucket_Web::HempyBucket_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor, WeightSensor *WasteReservoirWeightSensor, WaterPump *BucketPump) : Common(Name), Common_Web(Name), HempyBucket(Name, Parent, DefaultSettings, BucketWeightSensor, WasteReservoirWeightSensor, BucketPump)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void HempyBucket_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/S", 2) == 0)
  {
    ;
  }
  else
  {
    WebServer.setArgString(getName(F("ET"),true), getEvaporationTargetText());
    WebServer.setArgString(getName(F("OF"),true), getOverflowTargetText());
    WebServer.setArgString(getName(F("WL"),true), getWasteLimitText());
    WebServer.setArgInt(getName(F("D"),true), getDrainWaitTime());
    WebServer.setArgString(getName(F("DW"),true), getDryWeightText());
    //WebServer.setArgInt(getName(F("PS"),true), HempyBucketCommand1ToSend.PumpSpeed);
    //WebServer.setArgInt(getName(F("T"),true), HempyBucketCommand1ToSend.PumpTimeOut);
  }
}

void HempyBucket_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/S", 2) == 0) ///< When the settings page is refreshed
  {
    //WebServer.setArgString(getName(F("OS"),true), toText(*BucketWeightSensor->Offset));
    //WebServer.setArgString(getName(F("SC"),true), toText(*BucketWeightSensor->Scale));
    ;
  }
  else
  {
    //WebServer.setArgFloat(getName(F("W"),true), BucketWeightSensor->getWeightText());
    //WebServer.setArgString(getName(F("W"),true), toText_weight(HempyBucketResponse1Received.WeightB));
    //WebServer.setArgString(getName(F("WR"),true), toText_weight(HempyBucketResponse1Received.WeightWR));
    WebServer.setArgString(getName(F("DWW"),true), toText(getDryWeight(), "/", getWetWeight()));
    WebServer.setArgString(getName(F("S"),true), getStateText(true));
    //WebServer.setArgString(getName(F("P"),true), toText_waterPumpState(HempyBucketResponse1Received.PumpState));
  }
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/

bool HempyBucket_Web::commandEvent(char *Command, char *Data)
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
      Parent->addToLog(getName(F("watering")));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Off")) == 0)
    {
      stopWateringRequest();
      Parent->addToLog(getName(F("stopping")));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Dis")) == 0)
    {
      disableRequest();
      Parent->addToLog(getName(F("disabled")));
    }
    /*  
    else if (strcmp_P(ShortMessage, (PGM_P)F("TareB")) == 0)
    {
      HempyBucketCommand2ToSend.TareWeightB = true;
      Parent->addToLog(F("Taring Bucket 2 scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("TareDW")) == 0)
    {
      HempyBucketCommand2ToSend.TareWeightDW = true;
      Parent->addToLog(F("Taring Bucket 2 Dry/Wet"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("TareWR")) == 0)
    {
      HempyBucketCommand2ToSend.TareWeightWR = true;
      Parent->addToLog(F("Taring Bucket 2 waste scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("ET")) == 0)
    {
      DefaultSettings->EvaporationTarget_ = toFloat(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OF")) == 0)
    {
      DefaultSettings->OverflowTarget_ = toFloat(Data);
      Parent->addToLog(F("Bucket 2 targets updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("WL")) == 0)
    {
      DefaultSettings->WasteLimit_ = toFloat(Data);
      Parent->addToLog(F("Bucket 2 waste limit updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PS")) == 0)
    {
      DefaultSettings->PumpSpeed_ = toInt(Data);
      Parent->addToLog(F("Pump 2 speed updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("T")) == 0)
    {
      DefaultSettings->PumpTimeOut_ = toInt(Data);
      Parent->addToLog(F("Pump 2 timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("D")) == 0)
    {
      DefaultSettings->DrainWaitTime_ = toInt(Data);
      Parent->addToLog(F(" Drain wait updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("DW")) == 0)
    {
      HempyBucketCommand2ToSend.DryWeight = toFloat(Data);
      Parent->addToLog(F(" dry weight updated"), false);
    }

    //Settings
    else if (strcmp_P(ShortMessage, (PGM_P)F("Calibrate")) == 0)
    {
      BucketWeightSensor->triggerCalibration(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Scale")) == 0)
    {
      BucketWeightSensor->setScale(WebServer.getArgFloat());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Tare")) == 0)
    {
      BucketWeightSensor->tareRequest();
    }
    WebServer.setArgString(getName(F("DWW")), getDryWeightText(), getWetWeightText(), "/"));
    */   
    return true; //Match found
  }
}