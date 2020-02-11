#pragma once

#include "420Common_Web.h"
#include "../Modules/420Module_Web.h"
#include "../Components/PHSensor.h"

class PHSensor_Web : public PHSensor, public Common_Web
{
public:
 PHSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::PHSensorSettings *DefaultSettings);
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
  <h1>PHSensor_Web1</h1>
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">PH</span></th>			
    </tr>
    <tr>
      <td id="PHSensor_Web1_PH" class="dataRows"></td>
    </tr>
  </table>	
</div>

<div class="card" style="width:90%">
  <h1>Calibration</h1>				
  <b>PH sensor </b><button id="PHSensor_Web1_ReadRaw" type="button" style="margin-top:0px" title="Displays the analog PH value for a calibration point">Read raw</button>
  <form>
    Slope:<input style="width: 80px;" name="PHSensor_Web1_Slope" type="text"/> 
    Intercept:<input style="width: 80px;" name="PHSensor_Web1_Intercept" type="text"/>
    <input type="submit" value="Set">										
  </form>  			
</div>

*/