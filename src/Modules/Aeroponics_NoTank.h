#pragma once

#include "../../420Common.h"
#include "Aeroponics.h"

class GrowBox; //forward declaration

class Aeroponics_NoTank : public Aeroponics
{
public:
  Aeroponics_NoTank(const __FlashStringHelper *Name, GrowBox *GBox, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_NoTankSpecific *NoTankSpecificSettings, PressureSensor *FeedbackPressureSensor); //constructor
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(__attribute__((unused)) char *Button);
  void refresh_Sec();
  void report();
  float LastSprayPressure = 0; //tracks the last average pressure during a spray cycle

private:
  void bypassOn();
  void bypassOff();
  void sprayNow(bool FromWebsite = false);
  void sprayOff();
  char *sprayStateToText();
  int *BlowOffTime;               //After spraying open the bypass valve for X seconds to release pressure in the system
  bool BlowOffInProgress = false; //Aeroponics - True while bypass valve is open during a pressure blow-off. Only used without the Pressure Tank option.

protected:
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1 style="margin:0px">Aeroponics</h1>
  <font size="-1">Without pressure tank</font>
  <table class="tg">
    <tr>
    <th class="headerRows"><span style="font-weight:bold">Spray</span></th>
    <th class="headerRows"><span style="font-weight:bold">Last spray</br>pressure</span></th>
    <th class="headerRows"><span style="font-weight:bold">Current</br>pressure</span></th>
    <th class="headerRows"><span style="font-weight:bold">Pump</span></th>
    <th class="headerRows"><span style="font-weight:bold">Bypass</span></th>							
    </tr>
    <tr>
    <td id="Aero_NT1_SprayEnabled" class="dataRows"></td>
    <td id="Aero_NT1_SprayPressure" class="dataRows"></td>
    <td id="Aero_NT1_Pressure" class="dataRows"></td>
    <td id="Aero_NT1_PumpState" class="dataRows"></td>
    <td id="Aero_NT1_BypassState" class="dataRows"></td>				
    </tr>
  </table>
  <b>Spray: </b>
  <button id="Aero_NT1_SprayEnable" type="button" align="right" title="Enable spray cycle timer">Enable</button>
  <button id="Aero_NT1_SprayDisable" type="button" align="right" title="Disable spray cycle timer">Disable</button>
  <button id="Aero_NT1_SprayNow" type="button" title="Spray now and re-enable timer">Spray</button>
  <button id="Aero_NT1_SprayOff" type="button" title="Stop spraying">OFF</button>				
  <form>
    Interval: <input style="width: 35px;" min=1 name="Aero_NT1_Interval" type="number"/>min, 
    Duration: <input style="width: 35px;" min=1 name="Aero_NT1_Duration" type="number"/>sec
    <input type="submit" value="Set" title="Set how often a spray should start (Interval), and how long one spray should last (Duration)">	
  </form>	
  <b>Pump: </b>
  <button id="Aero_NT1_PumpOn" type="button" align="right" title="Starts the pump and reset its status to OK">ON</button>
  <button id="Aero_NT1_PumpOff" type="button" align="right" title="Stops the pump and reset its status to OK">OFF</button>
  <button id="Aero_NT1_PumpDisable" type="button" align="right" title="Stops and disables pump">Disable</button>
  <button id="Aero_NT1_Mix" type="button" align="right" title="Mix nutrients: Starts pump and opens bypass valve">Mix</button><br>
  <form>Timeout: <input style="width: 55px;" min=1 max=9999 name="Aero_NT1_PumpTimeout" type="number" title="Maximum time the pump can run continuously"/>min <input type="submit" value="Set"></form>
  <form>Priming time: <input style="width: 55px;" min=1 max=9999 name="Aero_NT1_PrimingTime" type="number" title="At pump startup the bypass valve is open for X seconds"/>sec <input type="submit" value="Set"></form>				
  </div>			
*/