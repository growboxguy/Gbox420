#include "WeightSensor.h"
#include "../../GrowBox.h"

WeightSensor::WeightSensor(const __FlashStringHelper *Name, GrowBox *GBox, Settings::WeightSensorSettings *DefaultSettings) : Common(Name)
{
  this->GBox = GBox;
  Scale = &DefaultSettings->Scale;
  TareOffset = &DefaultSettings->TareOffset;
  Weight = new RollingAverage();
  Sensor = new HX711();
  Sensor -> begin(*(&DefaultSettings->DTPin), *(&DefaultSettings->SCKPin));
  Sensor -> set_scale(*Scale);
  Sensor -> set_offset(*TareOffset);  
  GBox->AddToReportQueue(this);         //Subscribing to the report queue: Calls the report() method
  GBox->AddToRefreshQueue_Minute(this); //Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  GBox->AddToRefreshQueue_Sec(this);
  GBox->AddToWebsiteQueue_Load(this);   //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  GBox->AddToWebsiteQueue_Refresh(this);   //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  GBox->AddToWebsiteQueue_Button(this); //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  GBox->AddToWebsiteQueue_Field(this);  //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
  logToSerials(F("Weight Sensor object created"), true, 1);
}

void WeightSensor::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/Settings.html.json") == 0)
  {
    //
  }
}

void WeightSensor::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/Settings.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("TareOffset")), toText(*TareOffset));
    WebServer.setArgString(getWebsiteComponentName(F("Scale")), toText(*Scale));
  }
}

void WeightSensor::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Tare")) == 0)
    {
      triggerTare();
    }
  }
}

void WeightSensor::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Calibrate")) == 0)
    {
      triggerCalibration(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Scale")) == 0)
    {
      setScale(WebServer.getArgFloat());
    }
  }
}

void WeightSensor::refresh_Minute()
{
  if (GBox->BoxSettings->DebugEnabled)
    Common::refresh_Minute();
  if(TareRequested){
    TareRequested = false;
    tare();
  }
  if(CalibrateRequested){
    CalibrateRequested = false;
    calibrate();
  }
  readWeight();
}

void WeightSensor::refresh_Sec()
{
  refresh_Minute(); //temporary for faster testing
}

void WeightSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("Weight:"));
  strcat(LongMessage, getWeightText(true, true));
  logToSerials(&LongMessage, true, 1);
}

void WeightSensor::readWeight(){
  Weight -> updateAverage(Sensor -> get_units());
}

float WeightSensor::getWeight(bool ReturnAverage)
{
  return Weight->getFloat(ReturnAverage);
}

char *WeightSensor::getWeightText(bool IncludeUnits, bool ReturnAverage)
{
  if (IncludeUnits)
    return weightToText(Weight->getFloat(ReturnAverage));
  else
    return Weight->getFloatText(ReturnAverage);
}

void WeightSensor::triggerTare(){
  TareRequested = true;
  GBox->addToLog(F("Updating tare...")); //This can take up to 1 minute, when the component is next refreshed
}

void WeightSensor::tare() //Time intense, cannot be called straight from the website. Response would time out.
{
  Sensor -> tare();
  *TareOffset = Sensor -> get_offset();
  Weight->resetAverage();
  GBox->addToLog(F("Tare updated"));
}

void WeightSensor::triggerCalibration(int CalibrationWeight){
  this -> CalibrationWeight = CalibrationWeight;
  CalibrateRequested = true;
  GBox->addToLog(F("Calibrating weight..")); //This can take up to 1 minute, when the component is next refreshed
}

void WeightSensor::calibrate() //Time intense, cannot be called straight from the website. Response would time out.
{
  *Scale = (float) Sensor -> get_value() / CalibrationWeight;
  Sensor -> set_scale(*Scale);
  Weight->resetAverage();
  GBox->addToLog(F("Weight calibrated"));
}

void WeightSensor::setScale(float NewScale)
{
  *Scale = NewScale;
  Sensor -> set_scale(*Scale);
}