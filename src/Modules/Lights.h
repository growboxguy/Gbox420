#pragma once

#include "420Common.h"

class GrowBox; //forward declaration

class Lights : public Common
{
public:
  //Lights(GrowBox * GBox,byte RelayPin, byte DimmingPin, byte * DimmingLimit, bool *Status, byte *Brightness, bool *TimerEnabled, byte *OnHour, byte *OnMinute, byte *OffHour, byte *OffMinute);  //constructor
  Lights(const __FlashStringHelper *Name, GrowBox *GBox, Settings::LightsSettings *DefaultSettings); //constructor
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void refresh_Minute();
  void report();
  void setBrightness(byte Brightness, bool AddToLog);
  void setLightOnOff(bool State, bool AddToLog);
  bool getStatus();
  int getBrightness();
  char *getBrightnessText();
  char *getTimerOnOffText(bool UseText);
  char *getStatusText(bool UseText);
  char *getOnTimeText();
  char *getOffTimeText();

private:
  GrowBox *GBox; //Pointer to the GrowBox object that contains the Lights object
  void checkLightStatus();
  void checkLightTimer();
  void setTimerOnOff(bool State);
  void setOnHour(byte OnHour);
  void setOnMinute(byte OnMinute);
  void setOffHour(byte OffHour);
  void setOffMinute(byte OffMinute);
  byte *RelayPin;     //the Arduino pin controlling the AC relay
  byte *DimmingPin;   //PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
  byte *DimmingLimit; //Sets the LED dimming limit (Usually around 5%)
  bool *Status;       //Lights ON or OFF
  bool *TimerEnabled; //Enable timer controlling lights: true - Timer enabled, false - Timer disabled
  byte *OnHour;       //Light ON time - hour
  byte *OnMinute;     //Light ON time - minute
  byte *OffHour;      //Light OFF time - hour
  byte *OffMinute;    //Light OFF time - minute
  byte *Brightness;   //Light intensity: 0 - 100 range for controlling led driver output

protected:
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">		
  <h1>Lights</h1>
  <table class="tg">
    <tr>
    <td class="headerRows"><span style="font-weight:bold">Lights</span><br></td>
    <td class="headerRows"><span style="font-weight:bold">Timer</span></td>
    <td class="headerRows"><span style="font-weight:bold">Light ON</span></td>
    <td class="headerRows"><span style="font-weight:bold">Light OFF</span></td>
    </tr>
    <tr>
    <td id="Light1_Status" class="dataRows"></td>
    <td id="Light1_TimerEnabled" class="dataRows"></td>
    <td id="Light1_OnTime" class="dataRows"></td>
    <td id="Light1_OffTime" class="dataRows"></td>
    </tr>
    </table> 				
  <button id="Light1_On" type="button" title="Turn grow lights ON">ON</button>
  <button id="Light1_Off" type="button" title="Turn grow lights OFF">OFF</button><br>
  <b>Brightness</b><br>				
  <form>
  <input style="width:42px;" type="number" name="Light1_Brightness" min="0" max="100" oninput="this.form.Light1_BrightnessSlider.value=this.value" />
  <input type="range" name="Light1_BrightnessSlider" min="0" max="100" oninput="this.form.Light1_Brightness.value=this.value">
  <input type="submit" value="Set">				
  </form>				
  <b>Timer: </b>
  <button id="Light1_TimerEnable" type="button" align="right" title="Enable light timer">Enable</button>
  <button id="Light1_TimerDisable" type="button" align="right" title="Disable light timer">Disable</button>
  <form>ON : <input style="width: 35px;" min=0 max=23 name="Light1_OnHour" type="number"/>:<input style="width: 35px;" min=0 max=59 name="Light1_OnMinute" type="number"/> <input type="submit" value="Set" title="Set grow light ON time"></form>
  <form>OFF:<input style="width: 35px;" min=0 max=23 name="Light1_OffHour" type="number"/>:<input style="width: 35px;" min=0 max=59 name="Light1_OffMinute" type="number"/> <input type="submit" value="Set" title="Set grow light OFF time"></form>
</div>
*/