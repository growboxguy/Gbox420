#pragma once

#include "420Common_Web.h"
#include "../Components/WeightSensor.h"

class WeightSensor_Web : public WeightSensor
{
public:
  WeightSensor_Web(const __FlashStringHelper *Name, Module *Parent, Settings::WeightSensorSettings *DefaultSettings);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  
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
      <th class="headerRows"><span style="font-weight:bold">Weight</span></th>	
    </tr>
    <tr>
      <td id="Weight1_Weight" class="dataRows"></td>
    </tr>
  </table>	
</div>

<div class="card" style="width:90%">
  <h1>Calibration</h1>				
  <b>Weight sensor </b><button id="Weight1_ReadOffset" type="button" style="margin-top:0px" title="Use only when the tank is not pressurized!">Read offset</button>
  <form>
    Offset:<input style="width: 80px;" name="Weight1_Offset" type="text"/> 
    Ratio: <input style="width: 80px;" title="Voltage to Weight ratio" name="Weight1_Ratio" type="text"/>					
    <input type="submit" value="Set">					
  </form>				
</div>
*/