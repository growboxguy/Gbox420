#include "GrowBox.h"
#include "HempyModule_Web.h"
#include "AeroModule_Web.h"
#include "../Components/WaterPump.h"
#include "../Components_Web/DHTSensor_Web.h"
#include "../Components_Web/Lights_Web.h"
#include "../Components_Web/Sound_Web.h"
#include "../Components_Web/Fan_Web.h"
#include "../Components_Web/PowerSensor_Web.h"
#include "../Components_Web/PowerSensorV3_Web.h"  ///Only for PZEM004T V3.0
#include "../Components_Web/LightSensor_Web.h"
#include "../Components_Web/PHSensor_Web.h"
#include "../Components_Web/PressureSensor_Web.h"
#include "../Components_Web/Aeroponics_Tank_Web.h"
#include "../Components_Web/Aeroponics_NoTank_Web.h"
#include "../Components_Web/WaterLevelSensor_Web.h"
#include "../Components_Web/WaterTempSensor_Web.h"
//#include "../Components_Web/WeightSensor_Web.h"
#include "../Components_Web/ModuleSkeleton_Web.h" ///Only for demonstration purposes

GrowBox::GrowBox(const __FlashStringHelper *Name, Settings::GrowModuleSettings *DefaultSettings, RF24 *Wireless) : Common(Name), Common_Web(Name), Module_Web(Wireless)
{ ///Constructor
  this->Name = Name;
  SheetsReportingFrequency = &DefaultSettings-> SheetsReportingFrequency;
  ReportToGoogleSheets = &DefaultSettings-> ReportToGoogleSheets; 
  Sound1 = new Sound_Web(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this -> SoundFeedback = Sound1;
  IFan = new Fan_Web(F("IFan"), this, &ModuleSettings->IFan);      ///passing: Component name, GrowBox object the component belongs to, Default settings)
  EFan = new Fan_Web(F("EFan"), this, &ModuleSettings->EFan);
  Lt1 = new Lights_Web(F("Lt1"), this, &ModuleSettings->Lt1);
  LtSen1 = new LightSensor_Web(F("LtSen1"), this, &ModuleSettings->LtSen1, Lt1); ///Passing an extra Light object as parameter: Calibrates the light sensor against the passed Light object
  Pow1 = new PowerSensor_Web(F("Pow1"), this, &Serial2);
  ///Pow1 = new PowerSensorV3_Web(F("Pow1"), this, &Serial2); ///Only for PZEM004T V3.0
  IDHT = new DHTSensor_Web(F("IDHT"), this, &ModuleSettings->IDHT);
  EDHT = new DHTSensor_Web(F("EDHT"), this, &ModuleSettings->EDHT);
  Pres1 = new PressureSensor_Web(F("Pres1"), this, &ModuleSettings->Pres1);
  AeroPump1 = new WaterPump(F("AeroPump1"),this,&ModuleSettings->AeroPump1);
  //AeroT1 = new Aeroponics_Tank_Web(F("AeroT1"), this, &ModuleSettings->AeroT1_Common, &ModuleSettings->AeroT1_Specific, Pres1, AeroPump1); ///Passing the pressure sensor object that monitors the pressure inside the Aeroponics system
  AeroNT1 = new Aeroponics_NoTank_Web(F("AeroNT1"), this, &ModuleSettings->AeroNT1_Common, Pres1, AeroPump1);
  PHSensor1 = new PHSensor_Web(F("PHSensor1"), this, &ModuleSettings->PHSensor1);
  WaterTemp1 = new WaterTempSensor_Web(F("WaterTemp1"), this, &ModuleSettings->WaterTemp1);
  WaterLevel1 = new WaterLevelSensor_Web(F("WaterLevel1"), this, &ModuleSettings->WaterLevel1);
  HempyModule1 = new HempyModule_Web(F("Hemp1"), this,&ModuleSettings->HempyModule1);

  AeroModule1 = new AeroModule_Web(F("Aero1"), this,&ModuleSettings->AeroModule1);
  //Weight1 = new WeightSensor_Web(F("Weight1"), this, &ModuleSettings->Weight1);
  //Weight2 = new WeightSensor_Web(F("Weight2"), this, &ModuleSettings->Weight2);
  ///ModuleSkeleton1 = new ModuleSkeleton_Web(F("ModuleSkeleton1"),this,&ModuleSettings -> ModuleSkeleton1);  ///Only for demonstration purposes
  ///ModuleSkeleton2 = new ModuleSkeleton_Web(F("ModuleSkeleton2"),this,&ModuleSettings -> ModuleSkeleton2);  ///Only for demonstration purposes

  addToRefreshQueue_FiveSec(this);     ///Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  addToRefreshQueue_Minute(this);      ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  addToRefreshQueue_QuarterHour(this); ///Subscribing to the 30 minutes refresh queue: Calls the refresh_QuarterHour() method
  addToWebsiteQueue_Load(this);        ///Subscribing to the Website load event
  addToWebsiteQueue_Refresh(this);     ///Subscribing to the Website refresh event
  addToWebsiteQueue_Field(this);       ///Subscribing to the Website field submit event
  addToWebsiteQueue_Button(this);      ///Subscribing to the Website button press event
  logToSerials(F("GrowBox object created, refreshing..."), true, 0);
  runAll();
  addToLog(F("GrowBox initialized"), 0);
}

void GrowBox::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/S",2) == 0)
  {
    WebServer.setArgInt(getComponentName(F("Debug")), *Debug);
    WebServer.setArgInt(getComponentName(F("Metric")), *Metric);
    WebServer.setArgBoolean(getComponentName(F("Sheets")), *ReportToGoogleSheets);
    WebServer.setArgInt(getComponentName(F("SheetsF")), *SheetsReportingFrequency);
    WebServer.setArgString(getComponentName(F("Relay")), ModuleSettings -> PushingBoxLogRelayID);
  }
}

void GrowBox::websiteEvent_Refresh(__attribute__((unused)) char *url) ///called when website is refreshed.
{
  WebServer.setArgString(F("Time"), getFormattedTime(false));
  WebServer.setArgJson(F("Log"), eventLogToJSON()); ///Last events that happened in JSON format
}

void GrowBox::websiteEvent_Button(char *Button)
{ ///When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("SheetsRep")) == 0)
    {
      ReportToGoogleSheetsRequested = true;  ///just signal that a report should be sent, do not actually run it: Takes too long from an interrupt
      addToLog(F("Reporting to Sheets"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SerialRep")) == 0)
    {
      ConsoleReportRequested = true;
      addToLog(F("Reporting to Serial"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Refresh")) == 0) ///Website signals to refresh all sensor readings
    {        
      RefreshAllRequested = true;
      addToLog(F("Refresh triggered"), false);
    } 
  }
}

void GrowBox::websiteEvent_Field(char *Field)
{ ///When the website field is submitted
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Debug")) == 0){setDebug(WebServer.getArgBoolean());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("Metric")) == 0){setMetric(WebServer.getArgBoolean());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("Sheets")) == 0){setSheetsReportingOnOff(WebServer.getArgBoolean());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("SheetsF")) == 0){setSheetsReportingFrequency(WebServer.getArgInt());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("PushingBoxLogRelayID")) == 0){setPushingBoxLogRelayID(WebServer.getArgString());}
  }
}

void GrowBox::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  if (RefreshAllRequested)
  {
    RefreshAllRequested = false;
    runAll();
  }
  if(ReportToGoogleSheetsRequested)
  {
    ReportToGoogleSheetsRequested = false;
    reportToGoogleSheets(true);
  }
  if(ConsoleReportRequested)
  {
    ConsoleReportRequested = false;
    runReport();
  }
  
}

void GrowBox::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  runReport();
}

void GrowBox::refresh_QuarterHour()
{
  if (*Debug)
    Common::refresh_QuarterHour();
  reportToGoogleSheetsTrigger();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Settings
void GrowBox::setDebug(bool DebugEnabled)
{
  *Debug = DebugEnabled;
  if (*Debug)
  {
    addToLog(F("Debug enabled"));
    getSoundObject()->playOnSound();
  }
  else
  {
    addToLog(F("Debug disabled"));
    getSoundObject()->playOffSound();
  }
}

void GrowBox::setMetric(bool MetricEnabled)
{
  if (MetricEnabled != *Metric)
  { ///if there was a change
    *Metric = MetricEnabled;
    ///ModuleSettings -> IFanSwitchTemp = convertBetweenTempUnits(ModuleSettings -> IFanSwitchTemp);
    Pres1->Pressure->resetAverage();
    RefreshAllRequested = true;
  }
  if (*Metric)
    addToLog(F("Using Metric units"));
  else
    addToLog(F("Using Imperial units"));
  getSoundObject()->playOnSound();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///Google Sheets reporting

void GrowBox::setSheetsReportingOnOff(bool State)
{
  *ReportToGoogleSheets = State;
  if (State)
  {
    addToLog(F("Google Sheets enabled"));
    getSoundObject()->playOnSound();
  }
  else
  {
    addToLog(F("Google Sheets disabled"));
    getSoundObject()->playOffSound();
  }
}

void GrowBox::setSheetsReportingFrequency(uint8_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog(F("Reporting freqency updated"));
  getSoundObject()->playOnSound();
}

void GrowBox::reportToGoogleSheetsTrigger()
{ ///Handles custom reporting frequency for Google Sheets, called every 15 minutes
  if (SheetsRefreshCounter == 96)
    SheetsRefreshCounter = 0; ///Reset the counter after one day (15 x 96 = 1440 = 24 hours)
  if (SheetsRefreshCounter++ % (*SheetsReportingFrequency / 15) == 0)
  {
    reportToGoogleSheets(false);
  }
}

void GrowBox::reportToGoogleSheets(__attribute__((unused)) bool CalledFromWebsite)
{
  if (*ReportToGoogleSheets || CalledFromWebsite)
  {
    memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
    strcat_P(LongMessage, (PGM_P)F("{\"Log\":{"));
    strcat_P(LongMessage, (PGM_P)F("\"InternalTemp\":\""));
    strcat(LongMessage, IDHT->getTempText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"ExternalTemp\":\""));
    strcat(LongMessage, EDHT->getTempText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"InternalHumidity\":\""));
    strcat(LongMessage, IDHT->getHumidityText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"ExternalHumidity\":\""));
    strcat(LongMessage, EDHT->getHumidityText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"InternalFan\":\""));
    strcat(LongMessage, IFan->fanSpeedToNumber());
    strcat_P(LongMessage, (PGM_P)F("\",\"ExhaustFan\":\""));
    strcat(LongMessage, EFan->fanSpeedToNumber());
    strcat_P(LongMessage, (PGM_P)F("\",\"Lt1_Status\":\""));
    strcat(LongMessage, Lt1->getStatusText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"Lt1_Brightness\":\""));
    strcat(LongMessage, Lt1->getBrightnessText());
    strcat_P(LongMessage, (PGM_P)F("\",\"LightReading\":\""));
    strcat(LongMessage, LtSen1->getReadingText(true));
    strcat_P(LongMessage, (PGM_P)F("\",\"Dark\":\""));
    strcat(LongMessage, LtSen1->getDarkText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"WaterLevel\":\""));
    strcat(LongMessage, WaterLevel1->getLevelText());
    strcat_P(LongMessage, (PGM_P)F("\",\"WaterTemp\":\""));
    strcat(LongMessage, WaterTemp1->getTempText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"PH\":\""));
    strcat(LongMessage, PHSensor1->getPHText(true));
    strcat_P(LongMessage, (PGM_P)F("\",\"Pressure\":\""));
    strcat(LongMessage, Pres1->getPressureText(false, true));
    strcat_P(LongMessage, (PGM_P)F("\",\"Power\":\""));
    strcat(LongMessage, Pow1->getPowerText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"Energy\":\""));
    strcat(LongMessage, Pow1->getEnergyText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"Voltage\":\""));
    strcat(LongMessage, Pow1->getVoltageText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"Current\":\""));
    strcat(LongMessage, Pow1->getCurrentText(false));
    ///strcat_P(LongMessage,(PGM_P)F("\",\"Frequency\":\""));  strcat(LongMessage,Pow1 -> getFrequencyText(false));   ///Only for PZEM004T V3.0
    ///strcat_P(LongMessage,(PGM_P)F("\",\"PowerFactor\":\""));  strcat(LongMessage,Pow1 -> getPowerFactorText());    ///Only for PZEM004T V3.0
    strcat_P(LongMessage, (PGM_P)F("\",\"Lt1_Timer\":\""));
    strcat(LongMessage, Lt1->getTimerOnOffText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"Lt1_OnTime\":\""));
    strcat(LongMessage, Lt1->getOnTimeText());
    strcat_P(LongMessage, (PGM_P)F("\",\"Lt1_OffTime\":\""));
    strcat(LongMessage, Lt1->getOffTimeText());
    strcat_P(LongMessage, (PGM_P)F("\",\"AeroInterval\":\""));
    strcat(LongMessage, AeroNT1->getSprayIntervalText());
    strcat_P(LongMessage, (PGM_P)F("\",\"AeroDuration\":\""));
    strcat(LongMessage, AeroNT1->getSprayDurationText());
    strcat_P(LongMessage, (PGM_P)F("\",\"AeroSprayPressure\":\""));
    strcat(LongMessage, toText(AeroNT1->LastSprayPressure));
    ///strcat_P(LongMessage,(PGM_P)F("\",\"AeroInterval\":\"")); strcat(LongMessage,AeroT1 -> getSprayInterval());
    ///strcat_P(LongMessage,(PGM_P)F("\",\"AeroDuration\":\"")); strcat(LongMessage,AeroT1 -> getSprayDuration());

    strcat_P(LongMessage, (PGM_P)F("\"},\"Settings\":{"));
    strcat_P(LongMessage, (PGM_P)F("\"Metric\":\""));
    strcat(LongMessage, toText(*Metric));
    strcat_P(LongMessage, (PGM_P)F("\"}}"));
    relayToGoogleSheets(Name, &LongMessage);
  }  
}
///This is how a sent out message looks like:
///{parameter={Log={"Report":{"InternalTemp":"20.84","ExternalTemp":"20.87","InternalHumidity":"38.54","ExternalHumidity":"41.87","InternalFan":"0","ExhaustFan":"0","Lt1_Status":"0","Lt1_Brightness":"15","LightReading":"454","Dark":"1","WaterLevel":"0","WaterTemp":"20.56","PH":"17.73","Pressure":"-0.18","Power":"-1.00","Energy":"-0.00","Voltage":"-1.00","Current":"-1.00","Lt1_Timer":"1","Lt1_OnTime":"04:20","Lt1_OffTime":"16:20","AeroInterval":"15","AeroDuration":"2"},"Settings":{"Metric":"1"}}}, contextPath=, contentLength=499, queryString=, parameters={Log=[Ljava.lang.Object;@60efa46b}, postData=FileUpload}

void GrowBox::setPushingBoxLogRelayID(const char *ID)
{
  strncpy(ModuleSettings -> PushingBoxLogRelayID, ID, MaxTextLength);
  addToLog(F("Sheets log relay ID updated"));
}

/*
void GrowBox::relayToGoogleSheets(const __FlashStringHelper *Title, char (*JSONData)[MaxLongTextLength])
{
  char ValueToReport[MaxLongTextLength] = "";
  strcat_P(ValueToReport, (PGM_P)F("/pushingbox?devid="));
  strcat(ValueToReport, ModuleSettings -> PushingBoxLogRelayID);
  strcat_P(ValueToReport, (PGM_P)F("&BoxData={\""));
  strcat_P(ValueToReport, (PGM_P)Title);
  strcat_P(ValueToReport, (PGM_P)F("\":"));
  strcat(ValueToReport, *JSONData);
  strcat_P(ValueToReport, (PGM_P)F("}"));
  if (*Debug)
  { ///print the report command to console
    logToSerials(F("api.pushingbox.com"), false, 4);
    logToSerials(&ValueToReport, true, 0);
  }
  PushingBoxRestAPI.get(ValueToReport); ///PushingBoxRestAPI will append http:///api.pushingbox.com/ in front of the command
}
*/