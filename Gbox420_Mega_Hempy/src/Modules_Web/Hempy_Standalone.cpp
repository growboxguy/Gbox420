#include "Hempy_Standalone.h"
#include "../Components_Web/Sound_Web.h"
#include "../Components_Web/DHTSensor_Web.h"
#include "../Components_Web/WeightSensor_Web.h"
#include "../Components_Web/WaterPump_Web.h"
#include "../Components_Web/HempyBucket_Web.h"

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
*/
Hempy_Standalone::Hempy_Standalone(const __FlashStringHelper *Name, Settings::Hempy_StandaloneSettings *DefaultSettings) : Common(Name), Common_Web(Name), Module(Name), Module_Web(Name)
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

  logToSerials(F(""), true, 0);                                                          //<Line break
  Sound1 = new Sound_Web(F("Sound1"), this, &ModuleSettings->Sound1);                    ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;                                                          ///< Pointer for child objects to use sound feedback
  DHT1 = new DHTSensor_Web(F("DHT1"), this, &ModuleSettings->DHT1);                      ///< Humidity and temp sensor (not necessary)
  B1W = new WeightSensor_Web(F("B1W"), this, &ModuleSettings->B1W);                      ///< Bucket 1 Weight sensor
  B2W = new WeightSensor_Web(F("B2W"), this, &ModuleSettings->B2W);                      ///< Bucket 2 Weight sensor
  NRW = new WeightSensor_Web(F("NRW"), this, &ModuleSettings->WRW);                      ///< Nutrient Reservoir Weight sensor
  WRW = new WeightSensor_Web(F("WRW"), this, &ModuleSettings->WRW);                      ///< Waste Reservoir Weight sensor
  B1P = new WaterPump_Web(F("B1P"), this, &ModuleSettings->B1P);                         ///< Bucket 1 pump
  B2P = new WaterPump_Web(F("B2P"), this, &ModuleSettings->B2P);                         ///< Bucket 2 pump
  Bucket1 = new HempyBucket_Web(F("B1"), this, &ModuleSettings->Bucket1, B1W, WRW, B1P); ///< Bucket 1
  Bucket2 = new HempyBucket_Web(F("B2"), this, &ModuleSettings->Bucket2, B2W, WRW, B2P); ///< Bucket 1
  addToReportQueue(this);                                                                //< Attach to the report event: When triggered the module reports to the Serial Console or to MQTT
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

/**
* @brief Called when an ESP-link website is loading
*/
void Hempy_Standalone::websiteEvent_Load(__attribute__((unused)) char *Url) ///< called when website is first opened
{
  WebServer.setArgString(F("Time"), getFormattedTime(false));  ///< Current time
  WebServer.setArgJson(F("Log"), eventLogToJSON(false, true)); ///< Last events that happened in JSON format
}

/**
* @brief Called when an ESP-link website is refreshing
*/
void Hempy_Standalone::websiteEvent_Refresh(__attribute__((unused)) char *Url) ///< called when website is refreshed (5sec automatic)
{
  ;
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
bool Hempy_Standalone::commandEvent(__attribute__((unused))char *Command,__attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    /*
    char TempStorage[MaxShotTextLength] = "";
    strncpy(TempStorage, ShortMessage, MaxShotTextLength);
    
    //B1
    if (Bucket1->commandEvent(TempStorage, Data))
    {
      ;
    }
    else if (Bucket2>commandEvent(TempStorage, Data))
    {
      ;
    }
    else
    {
      return false; //Nothing matched
    }
    */
    
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