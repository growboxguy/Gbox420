#pragma once

#define PZEM004_NO_SWSERIAL  //Singals to the PZEM004Tv30 module to use HardwareSerial instead of SoftwareSerial
#include "PZEM004Tv30.h" //3rd party module for the PZEM004T V3.0 power sensor
#include "../../420Common.h"

class GrowBox;  //forward declaration

class PowerSensorV3 : public Common
{  
  public:
    PowerSensorV3(const __FlashStringHelper * Name, GrowBox * Gbox,HardwareSerial * SerialPort);  //constructor  
    float Power; //Power sensor - W
    float Energy; //Power sensor - Wh Total consumption 
    float Voltage; //Power sensor - V
    float Current; //Power sensor - A
    float Frequency;
    float PowerFactor;
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    void refresh_Minute();  //Called when component should refresh its state
    void report();
    char * getPowerText(bool IncludeUnits);
    char * getEnergyText(bool IncludeUnits);   
    char * getVoltageText(bool IncludeUnits);
    char * getCurrentText(bool IncludeUnits);    
    char * getFrequencyText(bool IncludeUnits);
    char * getPowerFactorText();

  private:
    GrowBox * GBox; //Pointer to the GrowBox object that contains the Lights object
    PZEM004Tv30 * Sensor; //for PZEM004T V3.0 model

  protected:
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