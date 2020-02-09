#pragma once

#include "420Common_Web.h"
#include "../Components/WaterTempSensor.h"

class WaterTempSensor_Web : public WaterTempSensor
{
public:
  WaterTempSensor_Web(const __FlashStringHelper *Name, Module *Parent, Settings::WaterTempSensorSettings *DefaultSettings); //constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  
private:
  
protected:
  Module_Web *Parent;
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