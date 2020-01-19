#pragma once

//This module is currenly untested

#include "../../420Common.h"
#include "Aeroponics.h"

class GrowBox;  //forward declaration

class Aeroponics_Tank : public Aeroponics
{
  public:
    Aeroponics_Tank(const __FlashStringHelper * Name, GrowBox * GBox, Settings::AeroponicsSettings * DefaultSettings, Settings::AeroponicsSettings_TankSpecific * TankSpecificSettings, PressureSensor * FeedbackPressureSensor);
    void websiteEvent_Field(char * Field);
    void refresh_Sec();
    void report();
    byte * SpraySolenoidPin;
    bool SpraySolenoidOn = false; //Aeroponics - Controls the spray valve, set to true to spay at power on.
    bool PreviousPressureRead = true;
    float * PressureLow; //Aeroponics - Turn on pump below this pressure (bar)
    float * PressureHigh; //Aeroponics - Turn off pump above this pressure (bar)
    void checkRelays();    
    void sprayOff();
    void sprayNow(bool DueToHighPressure);
    void setPressureLow(float PressureLow);
    void setPressureHigh(float PressureHigh);
  
  private:

  protected:
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1 style="margin:0px">Aero_T1</h1>
  <font size="-1">With pressure tank</font>				
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">Pump state</span></th>
      <th class="headerRows"><span style="font-weight:bold">Spray</span></span></th>					
      <th class="headerRows"><span style="font-weight:bold">Bypass</span></span></th>				
    </tr>
    <tr>			
      <td id="Aero_T1_PumpState" class="dataRows"></td>
      <td id="Aero_T1_SprayEnabled" class="dataRows"></td>
      <td id="Aero_T1_Bypass" class="dataRows"></td>
    </tr>
  </table>
  <b>Pump</b><br>
    <button id="Aero_T1_PumpOn" type="button" align="right" title="Starts the pump and reset its status to OK">ON</button>
    <button id="Aero_T1_PumpOff" type="button" align="right" title="Stops the pump and reset its status to OK">OFF</button>
    <button id="Aero_T1_PumpDisable" type="button" align="right" title="Stops and disables pump">Disable</button>
    <button id="Aero_T1_Mix" type="button" align="right" title="Mix nutrients: Starts pump and opens bypass valve">Mix</button><br>
    <form>Pump timeout:<input style="width: 55px;" min=1 max=9999 name="Aero_T1_PumpTimeout" type="number" title="Maximum high pressure tank refill time, if reached pump is considered broken"/> min <input type="submit" value="Set"></form>
    <form>Pump priming time:<input style="width: 55px;" min=1 max=9999 name="Aero_T1_PrimingTime" type="number" title="At pump startup the bypass valve is open for X seconds"/> sec <input type="submit" value="Set"></form>				
  <b>Spray</b><br>
    <button id="Aero_T1_SprayEnable" type="button" align="right" title="Enable spray cycle timer">Enable</button>
    <button id="Aero_T1_SprayDisable" type="button" align="right" title="Disable spray cycle timer">Disable</button>
    <button id="Aero_T1_SprayNow" type="button" title="Spray now and re-enable timer">Spray</button>
    <button id="Aero_T1_SprayOff" type="button" title="Stop spraying">OFF</button>				
    <form>
      Interval: <input style="width: 50px;" min=1 name="Aero_T1_Interval" type="number"/> min<br>
      Duration: <input style="width: 50px;" min=1 name="Aero_T1_Duration" type="number"/> sec
      <input type="submit" value="Set" title="Set how often a spray should start (Interval), and how long one spray should last (Duration)">	
    </form>									
</div>
*/