#pragma once

#define PZEM004_NO_SWSERIAL //Singals to the PZEM004Tv30 module to use HardwareSerial instead of SoftwareSerial
#include "PZEM004Tv30.h"    //3rd party module for the PZEM004T V3.0 power sensor
#include "420Common_Web.h"
#include "../Components/PowerSensorV3.h"

class PowerSensorV3_Web : public PowerSensorV3
{
public:
  PowerSensorV3_Web(const __FlashStringHelper *Name, Module *Parent, HardwareSerial *SerialPort); //constructor
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
    </tr>
    <tr>
      <td id="Power1_Power" class="dataRows"></td>
      <td id="Power1_Energy" class="dataRows"></td>
      <td id="Power1_Voltage" class="dataRows"></td>
    </tr>
    <tr>
      <th class="headerRows"><span style="font-weight:bold">Current</span></th>
      <th class="headerRows"><span style="font-weight:bold">Frequency</span></th>
      <th class="headerRows"><span style="font-weight:bold">Power factor</span></th>						
    </tr>
    <tr>
      <td id="Power1_Current" class="dataRows"></td>
      <td id="Power1_Frequency" class="dataRows"></td>
      <td id="Power1_PowerFactor" class="dataRows"></td>
    </tr>					  
    </table>						 
</div>
*/