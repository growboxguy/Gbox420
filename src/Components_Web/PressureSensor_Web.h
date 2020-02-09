#pragma once

#include "420Common_Web.h"
#include "../Components/PressureSensor.h"

class GrowBox; //forward declaration

class PressureSensor_Web : public PressureSensor
{
public:
  PressureSensor_Web(const __FlashStringHelper *Name, Module *Parent, Settings::PressureSensorSettings *DefaultSettings);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);  

private:

protected:
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>WaterLevel1</h1>
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">Pressure</span></th>	
    </tr>
    <tr>
      <td id="Pressure1_Pressure" class="dataRows"></td>
    </tr>
  </table>	
</div>

<div class="card" style="width:90%">
  <h1>Calibration</h1>				
  <b>Pressure sensor </b><button id="Pressure1_ReadOffset" type="button" style="margin-top:0px" title="Use only when the tank is not pressurized!">Read offset</button>
  <form>
    Offset:<input style="width: 80px;" name="Pressure1_Offset" type="text"/> 
    Ratio: <input style="width: 80px;" title="Voltage to pressure ratio" name="Pressure1_Ratio" type="text"/>					
    <input type="submit" value="Set">					
  </form>				
</div>
*/