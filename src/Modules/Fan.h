#pragma once

#include "../../420Common.h"

class GrowBox;  //forward declaration

class Fan : public Common
{
  public:
    Fan(const __FlashStringHelper * Name, GrowBox * GBox, Settings::FanSettings * DefaultSettings);
    GrowBox * GBox;

    void refresh_Minute();  //Called when component should refresh its state
    void report();
    //void websiteEvent_Load(__attribute__((unused)) char * url);
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    void websiteEvent_Button(char * Button);
    //void websiteEvent_Field(char * Field);
    void checkFanStatus();
    void TurnOff();
    void SetLowSpeed();
    void SetHighSpeed();
    char * fanSpeedToText();
    char * fanSpeedToNumber();
  
  private:
    bool * State;
    bool * HighSpeed;     
    byte * OnOffPin;
    byte * SpeedPin;
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>Fan1</h1>		
  <table class="tg">				  
    <tr>
      <td class="headerRows"><span style="font-weight:bold">Internal fan</span></td>
    </tr>
    <tr>
      <td id="Fan1_Status" class="dataRows"></td>
    </tr>
  </table>
  <b>Fan :</b>
    <button id="Fan1_Off" type="button" title="Turn fan OFF">OFF</button>
    <button id="Fan1_Low" type="button" title="Turn fan ON and set speed to LOW">Low</button>
    <button id="Fan1_High" type="button" title="Turn fan ON and set speed to HIGH">High</button>
  <br>				
</div>	
*/