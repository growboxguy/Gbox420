#pragma once

#include "420Common_Web.h"
#include "../Modules/420Module_Web.h"
#include "../Components/LightSensor.h"

class GrowBox; //forward declaration

class LightSensor_Web : public LightSensor, public Common_Web
{
public:
  LightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  
private:
  
protected:
  Module_Web *Parent;;
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">		
  <h1>Light sensor</h1>
  <table class="tg">
    <tr>
    <td class="headerRows"><span style="font-weight:bold">Is dark</span></td>
    <td class="headerRows"><span style="font-weight:bold">Reading</span></td>
    <td class="headerRows"><span style="font-weight:bold">Dark/Min/Max</span></td>
    </tr>
    <tr>
    <td id="LightSensor_Web1_Dark" class="dataRows"></td>
    <td id="LightSensor_Web1_Reading" class="dataRows"></td>
    <td id="LightSensor_Web1_Calibrated" class="dataRows"></td>
    </tr>				 
  </table> 
  <button id="LightSensor_Web1_Calibrate" type="button" title="Measure light reading in dark, 0 and 100% brightness">Calibrate</button><br>
</div>	
*/