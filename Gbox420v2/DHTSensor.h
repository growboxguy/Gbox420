#pragma once

#include "DHT.h"  //DHT11 or DHT22 Digital Humidity and Temperature sensor
#include "420Common.h"

class GrowBox;  //forward declaration

class DHTSensor : public Common
{
  public:
    DHTSensor(const __FlashStringHelper * Name, GrowBox * GBox, Settings::DHTSensorSettings * DefaultSettings);
    void websiteEvent_Refresh(__attribute__((unused)) char * url);    
    void refresh_Minute();  //Called when component should refresh its state
    void report();
    float getTemp();  
    float getHumidity();
    char * getTempText(bool IncludeUnits);  
    char * getHumidityText(bool IncludeUnits);
    RollingAverage * Temp;
    RollingAverage * Humidity;
  
  private:
    GrowBox * GBox;
    DHT* Sensor; //Pointer declaration, points to null initially 
  
  protected:
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>DHT sensor</h1>		
  <table class="tg">				  
    <tr>
      <td class="headerRows"><span style="font-weight:bold">Internal<br>temp</span></td>
      <td class="headerRows"><span style="font-weight:bold">Internal<br>humidity</span></td>
      <td class="headerRows"><span style="font-weight:bold">External<br><span>temp</td>
      <td class="headerRows"><span style="font-weight:bold">External<br>humidity</span></td>
    </tr>
    <tr>
      <td id="InDHT_Temp" class="dataRows"></td>
      <td id="InDHT_Humidity" class="dataRows"></td>
      <td id="ExDHT_Temp" class="dataRows"></td>
      <td id="ExDHT_Humidity" class="dataRows"></td>
    </tr>
  </table>						
</div>		
*/