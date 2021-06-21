#include "Hempy_Standalone.h"
#include "../Components/Sound.h"
#include "../Components/DHTSensor.h"
#include "../Components/WeightSensor.h"
#include "../Components/WaterPump.h"
#include "../Components/HempyBucket.h"

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
*/
Hempy_Standalone::Hempy_Standalone(const __FlashStringHelper *Name, Settings::Hempy_StandaloneSettings *DefaultSettings) : Module_Web(Name), Common_Web(Name), Module(Name), Common(Name)
{
  SerialReportFrequency = &DefaultSettings->SerialReportFrequency;
  SerialReportDate = &DefaultSettings->SerialReportDate;
  SerialReportMemory = &DefaultSettings->SerialReportMemory;
  SerialReportJSONFriendly = &DefaultSettings->SerialReportJSONFriendly;
  SerialReportJSON = &DefaultSettings->SerialReportJSON;
  SerialReportWireless = &DefaultSettings->SerialReportWireless;
  ReportToGoogleSheets = &DefaultSettings->ReportToGoogleSheets;
  SheetsReportingFrequency = &DefaultSettings->SheetsReportingFrequency;
  ReportToMQTT = &DefaultSettings->ReportToMQTT;
  MQTTReportFrequency = &DefaultSettings->MQTTReportFrequency;

  logToSerials(F(""), true, 0);                                   //<Line break
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;                                   ///< Pointer for child objects to use sound feedback
  DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  WeightB1 = new WeightSensor(F("WeightB1"), this, &ModuleSettings->WeightB1);    //< Bucket 1 weight sensor
  WeightB2 = new WeightSensor(F("WeightB2"), this, &ModuleSettings->WeightB2);    //< Bucket 2 weight sensor
  WeightNR1 = new WeightSensor(F("WeightNR1"), this, &ModuleSettings->WeightWR1); //< Nutrient reservoir weight sensor
  WeightWR1 = new WeightSensor(F("WeightWR1"), this, &ModuleSettings->WeightWR1); //< Waste reservoir weight sensor
  Pump1 = new WaterPump(F("Pump1"), this, &ModuleSettings->Pump1);
  Pump2 = new WaterPump(F("Pump2"), this, &ModuleSettings->Pump2);
  Bucket1 = new HempyBucket(F("Bucket1"), this, &ModuleSettings->Bucket1, WeightB1, WeightWR1, Pump1);
  Bucket2 = new HempyBucket(F("Bucket2"), this, &ModuleSettings->Bucket2, WeightB2, WeightWR1, Pump2);
  addToReportQueue(this); //< Attach to the report event: When triggered the module reports to the Serial Console or to MQTT
  //addToRefreshQueue_Sec(this);     //< Attach to a trigger that fires every second and calls refresh_Sec()
  addToRefreshQueue_FiveSec(this); //< Attach to a trigger that fires every five seconds and calls refresh_FiveSec()
  addToRefreshQueue_Minute(this);  //< Attach to a trigger that fires every second and calls refresh_Minute()
  addToWebsiteQueue_Load(this);    //< Attach to the ESP-link website load event: Calls websiteEvent_Load() when an ESP-link webpage is opened
  addToWebsiteQueue_Refresh(this); //< Attach to the ESP-link website refresh event: Calls websiteEvent_Refresh() when an ESP-link webpage is refreshing
  addToCommandQueue(this);
  addToLog(F("Hempy_Standalone ready"), 0);
  logToSerials(Name, false, 0);
  logToSerials(F("refreshing"), true, 1);
  runAll();  
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void Hempy_Standalone::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"M\":\""));
  strcat(LongMessage, getMetricText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, getDebugText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void Hempy_Standalone::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0) //GrowBox tab
  {
    WebServer.setArgString(getComponentName(F("B1ET")), Bucket1->getEvaporationTargetText());
    WebServer.setArgString(getComponentName(F("B1OF")), Bucket1->getOverflowTargetText());
    WebServer.setArgString(getComponentName(F("B1WL")), Bucket1->getWasteLimitText());
    WebServer.setArgInt(getComponentName(F("B1PS")), Pump1->getSpeed());
    WebServer.setArgInt(getComponentName(F("B1T")), Pump1->getPumpTimeOut());
    WebServer.setArgInt(getComponentName(F("B1D")), Bucket1->getDrainWaitTime());
    WebServer.setArgString(getComponentName(F("B1DW")), Bucket1->getDryWeightText());
    WebServer.setArgString(getComponentName(F("B2ET")), Bucket2->getEvaporationTargetText());
    WebServer.setArgString(getComponentName(F("B2OF")), Bucket2->getOverflowTargetText());
    WebServer.setArgString(getComponentName(F("B2WL")), Bucket2->getWasteLimitText());
    WebServer.setArgInt(getComponentName(F("B2PS")), Pump2->getSpeed());
    WebServer.setArgInt(getComponentName(F("B2T")), Pump2->getPumpTimeOut());
    WebServer.setArgInt(getComponentName(F("B2D")), Bucket2->getDrainWaitTime());
    WebServer.setArgString(getComponentName(F("B2DW")), Bucket2->getDryWeightText());
  }
  else
  {
    Module_Web::websiteEvent_Load(url);
  }
}

void Hempy_Standalone::websiteEvent_Refresh(__attribute__((unused)) char *url) ///< called when website is refreshed.
{
  //All tabs
  WebServer.setArgString(getComponentName(F("Time")), getFormattedTime(false));
  WebServer.setArgJson(getComponentName(F("Log")), eventLogToJSON(false, true)); ///< Last events that happened in JSON format

  if (strncmp(url, "/G", 2) == 0) //GrowBox tab
  {
    //DHT1
    WebServer.setArgString(getComponentName(F("DT")), DHT1->getTempText(true)); ///< Shows the latest reading
    WebServer.setArgString(getComponentName(F("DH")), DHT1->getHumidityText(true));
    //Bucket 1
    WebServer.setArgString(getComponentName(F("B1W")), WeightB1->getWeightText(false, true));
    WebServer.setArgString(getComponentName(F("B1WR")), WeightWR1->getWeightText(false, true));
    WebServer.setArgString(getComponentName(F("B1DWW")), toText(Bucket1->getDryWeight(), Bucket1->getWetWeight(), "/"));
    WebServer.setArgString(getComponentName(F("B1S")), Bucket1->getStateText());
    WebServer.setArgString(getComponentName(F("B1P")), Pump1->getStateText());
    //Bucket 2
    WebServer.setArgString(getComponentName(F("B2W")), WeightB2->getWeightText(false, true));
    WebServer.setArgString(getComponentName(F("B2WR")), WeightWR1->getWeightText(false, true));
    WebServer.setArgString(getComponentName(F("B2DWW")), toText(Bucket2->getDryWeight(), Bucket2->getWetWeight(), "/"));
    WebServer.setArgString(getComponentName(F("B2S")), Bucket2->getStateText());
    WebServer.setArgString(getComponentName(F("B2P")), Pump2->getStateText());
  }
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
bool Hempy_Standalone::commandEvent(char *Command, char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    //Bucket1
    if (Bucket1->commandEvent(Command, Data))
    {
      ;
    }
    else if (Bucket2->commandEvent(Command, Data))
    {
      ;
    }
    else if (Module_Web::commandEvent(Command, Data))
    {
      ;
    }
    else
    {
      return false; //Nothing matched
    }
    return true; //Match found
  }
}

/*
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1On")) == 0)
    {
      HempyBucketCommand1ToSend.StartWatering = true;
      addToLog(F("Watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Off")) == 0)
    {
      HempyBucketCommand1ToSend.StopWatering = true;
      addToLog(F("Stop watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Dis")) == 0)
    {
      HempyBucketCommand1ToSend.Disable = true;
      addToLog(F("Disabled HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1TareB")) == 0)
    {
      HempyBucketCommand1ToSend.TareWeightB = true;
      addToLog(F("Taring Bucket 1 scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1TareDW")) == 0)
    {
      HempyBucketCommand1ToSend.TareWeightDW = true;
      addToLog(F("Taring Bucket 1 Dry/Wet"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1TareWR")) == 0)
    {
      HempyBucketCommand1ToSend.TareWeightWR = true;
      addToLog(F("Taring Bucket 1 waste scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1ET")) == 0)
    {
      DefaultSettings->EvaporationTarget_B1 = toFloat(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1OF")) == 0)
    {
      DefaultSettings->OverflowTarget_B1 = toFloat(Data);
      addToLog(F("Bucket 1 targets updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1WL")) == 0)
    {
      DefaultSettings->WasteLimit_B1 = toFloat(Data);
      addToLog(F("Bucket 1 waste limit updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1PS")) == 0)
    {
      DefaultSettings->PumpSpeed_B1 = toInt(Data);
      addToLog(F("Pump 1 speed updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1T")) == 0)
    {
      DefaultSettings->PumpTimeOut_B1 = toInt(Data);
      addToLog(F("Pump 1 timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1D")) == 0)
    {
      DefaultSettings->DrainWaitTime_B1 = toInt(Data);
      addToLog(F("B1 Drain wait updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1DW")) == 0)
    {
      HempyBucketCommand1ToSend.DryWeight = toFloat(Data);
      addToLog(F("B1 dry weight updated"), false);
    }
    //Bucket
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2On")) == 0)
    {
      HempyBucketCommand2ToSend.StartWatering = true;
      addToLog(F("Watering HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Off")) == 0)
    {
      HempyBucketCommand2ToSend.StopWatering = true;
      addToLog(F("Stop watering HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Dis")) == 0)
    {
      HempyBucketCommand2ToSend.Disable = true;
      addToLog(F("Disabled HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2TareB")) == 0)
    {
      HempyBucketCommand2ToSend.TareWeightB = true;
      addToLog(F("Taring Bucket 2 scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2TareDW")) == 0)
    {
      HempyBucketCommand2ToSend.TareWeightDW = true;
      addToLog(F("Taring Bucket 2 Dry/Wet"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2TareWR")) == 0)
    {
      HempyBucketCommand2ToSend.TareWeightWR = true;
      addToLog(F("Taring Bucket 2 waste scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2ET")) == 0)
    {
      DefaultSettings->EvaporationTarget_B2 = toFloat(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2OF")) == 0)
    {
      DefaultSettings->OverflowTarget_B2 = toFloat(Data);
      addToLog(F("Bucket 2 targets updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2WL")) == 0)
    {
      DefaultSettings->WasteLimit_B2 = toFloat(Data);
      addToLog(F("Bucket 2 waste limit updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PS")) == 0)
    {
      DefaultSettings->PumpSpeed_B2 = toInt(Data);
      addToLog(F("Pump 2 speed updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2T")) == 0)
    {
      DefaultSettings->PumpTimeOut_B2 = toInt(Data);
      addToLog(F("Pump 2 timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2D")) == 0)
    {
      DefaultSettings->DrainWaitTime_B2 = toInt(Data);
      addToLog(F("B2 Drain wait updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2DW")) == 0)
    {
      HempyBucketCommand2ToSend.DryWeight = toFloat(Data);
      addToLog(F("B2 dry weight updated"), false);
    }
    */

void Hempy_Standalone::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  reportToSerialTrigger();
  reportToMQTTTrigger();
  if (RefreshAllRequested)
  {
    RefreshAllRequested = false;
    runAll();
  }
  if (ReportToGoogleSheetsRequested)
  {
    ReportToGoogleSheetsRequested = false;
    reportToGoogleSheetsTrigger(true);
  }
  if (ConsoleReportRequested)
  {
    ConsoleReportRequested = false;
    runReport();
  }
  if (MQTTReportRequested)
  {
    MQTTReportRequested = false;
    reportToMQTTTrigger(true);
  }
}

void Hempy_Standalone::refresh_Minute()
{
  Common::refresh_Minute();
  reportToGoogleSheetsTrigger();
}