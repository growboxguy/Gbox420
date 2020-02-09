#pragma once

#include "420Common_Web.h"
#include "../Components/WaterLevelSensor.h"

class WaterLevelSensor_Web : public WaterLevelSensor
{
public:
  WaterLevelSensor_Web(const __FlashStringHelper *Name, Module *Parent, Settings::WaterLevelSensorSettings *DefaultSettings); //constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  
private:
  
protected:
  Module_Web *Parent;
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>WaterLevel1</h1>
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">Level</span></th>	
    </tr>
    <tr>
      <td id="WaterLevel1_Level" class="dataRows"></td>
    </tr>
  </table>	
</div>
*/