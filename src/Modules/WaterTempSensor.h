#pragma once

#include "OneWire.h" //DS18B20 waterproof temperature sensor
#include "DallasTemperature.h" //DS18B20 waterproof temperature sensor
#include "../../420Common.h"

class GrowBox;  //forward declaration

class WaterTempSensor : public Common
{
  public:
    WaterTempSensor(const __FlashStringHelper * Name, GrowBox * GBox, Settings::WaterTempSensorSettings * DefaultSettings);  //constructor
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    void refresh_Minute();
    void report();
    float getTemp(bool ReturnAverage = true);
    char * getTempText(bool IncludeUnits, bool ReturnAverage);
    RollingAverage * Temp;

  private:
    GrowBox * GBox; //Pointer to the GrowBox object that contains the Lights object
    OneWire * TempSensorWire;
    DallasTemperature * TempSensor;

  protected:         
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>WaterTemp1</h1>
  <table class="tg">
    <tr>				 
      <th class="headerRows"><span style="font-weight:bold">Temp</span></th>	
    </tr>
    <tr>
      <td id="WaterTemp1_Temp" class="dataRows"></td>
    </tr>
  </table>	
</div>
*/