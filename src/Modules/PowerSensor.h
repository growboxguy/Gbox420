#pragma once

#include "PZEM004T.h" //3rd party module for the PZEM004T power sensor
#include "420Common.h"

class GrowBox; //forward declaration

class PowerSensor : public Common
{
public:
  PowerSensor(const __FlashStringHelper *Name, GrowBox *Gbox, HardwareSerial *SerialPort); //constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void refresh_FiveSec();
  void report();
  char *getPowerText(bool IncludeUnits);
  char *getEnergyText(bool IncludeUnits);
  char *getVoltageText(bool IncludeUnits);
  char *getCurrentText(bool IncludeUnits);

private:
  GrowBox *GBox;            //Pointer to the GrowBox object that contains the Lights object
  PZEM004T *Sensor;         //for PZEM004T model
  IPAddress *PowerSensorIP; // Power Sensor address (fake,just needs something set)
  float Power;              //Power sensor - W
  float Energy;             //Power sensor - Wh Total consumption
  float Voltage;            //Power sensor - V
  float Current;            //Power sensor - A

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