#pragma once

#include "PZEM004T.h" //3rd party module for the PZEM004T power sensor
#include "420Common_Web.h"
#include "../Modules/420Module_Web.h"
#include "../Components/PowerSensor.h"

class PowerSensor_Web : public PowerSensor
{
public:
  PowerSensor_Web(const __FlashStringHelper *Name, Module *Parent, HardwareSerial *SerialPort); //constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  
private:
 
protected:
  Module_Web *Parent;
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>Power</h1>
    <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">Power</span></th>
      <th class="headerRows"><span style="font-weight:bold">Energy</span></th>
      <th class="headerRows"><span style="font-weight:bold">Voltage</span></th>	
      <th class="headerRows"><span style="font-weight:bold">Current</span></th>					
    </tr>
    <tr>
      <td id="Power1_Power" class="dataRows"></td>
      <td id="Power1_Energy" class="dataRows"></td>
      <td id="Power1_Voltage" class="dataRows"></td>
      <td id="Power1_Current" class="dataRows"></td>
    </tr>    
    </table>						 
</div>
*/