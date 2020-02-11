#pragma once
//This is a template for creating new modules

//#include "3rdPartyModlue.h" //Load any required 3rd party modules here
#include "420Common_Web.h"
#include "../Modules/420Module_Web.h"
#include "../Components/ModuleSkeleton.h"

class ModuleSkeleton_Web : public ModuleSkeleton, public Common_Web
{
public:
  ModuleSkeleton_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::ModuleSkeletonSettings *DefaultSettings); //constructor
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  
private:
  
protected:
  Module_Web *Parent;
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>ModulSkeleton1</h1>
  <h3>EEPROM stored - Presistent</h2>
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">PersitentBool</span></th>
      <th class="headerRows"><span style="font-weight:bold">PersitentInt</span></th>
      <th class="headerRows"><span style="font-weight:bold">PersitentFloat</span></th>				
    </tr>
    <tr>
      <td id="ModuleSkeleton_Web1_PersistentBool" class="dataRows"></td>
      <td id="ModuleSkeleton_Web1_PersistentInt" class="dataRows"></td>
      <td id="ModuleSkeleton_Web1_PersistentFloat" class="dataRows"></td>
    </tr>    
  </table>
  <button id="ModuleSkeleton_Web1_ResetPers" type="button" style="margin-top:3px;float:right" title="Refresh the page(F5) to see changes">Reset EEPROM</button>
  <form><input type="checkbox" name="ModuleSkeleton_Web1_SetPersBool">Bool <input type="submit" value="Set"></form>
  Change 2 values with one button:<br>	
  <form>
    Int: <input style="width: 45px;" min=0 max=1000 name="ModuleSkeleton_Web1_SetPersInt" type="number"/>
    Float: <input style="width: 80px;" name="ModuleSkeleton_Web1_SetPersFloat" type="text"/> 
    <input type="submit" value="Set">
  </form>
  <h3>RAM stored - Runtime</h3>
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">RuntimeBool</span></th>
      <th class="headerRows"><span style="font-weight:bold">RuntimeInt</span></th>
      <th class="headerRows"><span style="font-weight:bold">RuntimeFloat</span></th>				
    </tr>
    <tr>
      <td id="ModuleSkeleton_Web1_RuntimeBool" class="dataRows"></td>
      <td id="ModuleSkeleton_Web1_RuntimeInt" class="dataRows"></td>
      <td id="ModuleSkeleton_Web1_RuntimeFloat" class="dataRows"></td>
    </tr>    
  </table>
  <button id="ModuleSkeleton_Web1_ResetRun" type="button" style="margin-top:3px;float:right">Reset RAM</button>
  <form><label><input type="checkbox" name="ModuleSkeleton_Web1_SetRunBool">Auto Increment (5sec) </label><input type="submit" value="Set"></form>
  <br>
  <b>Word of the day is: </b><span id="ModuleSkeleton_Web1_RuntimeString"></span><br>
  <form>Change the word: <input type="text" name="ModuleSkeleton_Web1_SetRunString"  style="width: 150px;" title="Change the word of the day text"/> <input type="submit" value="Set"></form>
  <br>
  <b>Rolling average smoothing</b>
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">Rolling average Int</span></th>
      <th class="headerRows"><span style="font-weight:bold">Rolling average Float</span></th>					  			
    </tr>
    <tr>
      <td id="ModuleSkeleton_Web1_RollingInt" class="dataRows"></td>
      <td id="ModuleSkeleton_Web1_RollingFloat" class="dataRows"></td>
    </tr>    
  </table>				
  Add a value to Rolling Average:<br>	
  <form>Int: <input style="width: 40px;" name="ModuleSkeleton_Web1_SetRunInt" type="number"/ value='15'/> <input type="submit" value="Set"></form>
  <form>Float: <input style="width: 50px;" name="ModuleSkeleton_Web1_SetRunFloat" type="number"/ vstep='0.1' value='4.2'/> <input type="submit" value="Set"></form>	
  <button id="ModuleSkeleton_Web1_ResetRollInt" type="button" style="margin-top:3px">Reset RollingInt</button>
  <button id="ModuleSkeleton_Web1_ResetRollFloat" type="button" style="margin-top:3px">Reset RollingFloat</button>
</div>
*/