#pragma once

#include "OneWire.h" //DS18B20 waterproof temperature sensor
#include "DallasTemperature.h" //DS18B20 waterproof temperature sensor
#include "420Common.h"

class GrowBox;  //forward declaration

class WaterTempSensor : public Common
{
  protected: 

  public:
    WaterTempSensor(const __FlashStringHelper * Name, GrowBox * GBox, Settings::WaterTempSensorSettings * DefaultSettings);  //constructor
    //void websiteEvent_Load(__attribute__((unused)) char * url);
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    //void websiteEvent_Button(char * Button);
    //void websiteEvent_Field(char * Field);  
    void refresh_Minute();
    void report();
    float getTemp();
    char * getTempText(bool IncludeUnits);
    RollingAverage * Temp;

  private:
    GrowBox * GBox; //Pointer to the GrowBox object that contains the Lights object
    OneWire * TempSensorWire;
    DallasTemperature * TempSensor;
         
};