#pragma once

#include "420Common_Web.h"
#include "../Components/Fan.h"

class Fan_Web : public Fan
{
public:
  Fan_Web(const __FlashStringHelper *Name, Module *Parent, Settings::FanSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
 
private:
 
protected:
  Module_Web *Parent;
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