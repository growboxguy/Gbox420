#pragma once
//This is a template for creating new modules

//#include "3rdPartyModlue.h" //Load any required 3rd party modules here
#include "../420Common.h"

class GrowBox;  //forward declaration

class ModuleSkeleton : public Common
{
  public:
    ModuleSkeleton(const __FlashStringHelper * Name, GrowBox * GBox, Settings::ModuleSkeletonSettings * DefaultSettings);  //constructor
    void websiteEvent_Load(__attribute__((unused)) char * url);
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    void websiteEvent_Button(char * Button);
    void websiteEvent_Field(char * Field);  
    void refresh_Sec();
    void refresh_FiveSec();
    void refresh_Minute();
    void refresh_QuarterHour();
    void report();

    //Variables stored in memory, values reset at reboot
    bool RuntimeBool = false;
    int RuntimeInt = 420;
    float RuntimeFloat = 4.2f;
    char RuntimeString[MaxTextLength] = "HailMary";
    RollingAverage * RollingInt;
    RollingAverage * RollingFloat;
    
    //Variables stored in EEPROM, values kept between reboots
    bool * PersistentBool;
    int * PersistentInt;
    float * PersistentFloat;

  private:
    GrowBox * GBox; //Pointer to the GrowBox object that contains the Lights object
    
  protected:         
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
      <td id="ModuleSkeleton1_PersistentBool" class="dataRows"></td>
      <td id="ModuleSkeleton1_PersistentInt" class="dataRows"></td>
      <td id="ModuleSkeleton1_PersistentFloat" class="dataRows"></td>
    </tr>    
  </table>
  <button id="ModuleSkeleton1_ResetPers" type="button" style="margin-top:3px;float:right" title="Refresh the page(F5) to see changes">Reset EEPROM</button>
  <form><input type="checkbox" name="ModuleSkeleton1_SetPersBool">Bool <input type="submit" value="Set"></form>
  Change 2 values with one button:<br>	
  <form>
    Int: <input style="width: 45px;" min=0 max=1000 name="ModuleSkeleton1_SetPersInt" type="number"/>
    Float: <input style="width: 80px;" name="ModuleSkeleton1_SetPersFloat" type="text"/> 
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
      <td id="ModuleSkeleton1_RuntimeBool" class="dataRows"></td>
      <td id="ModuleSkeleton1_RuntimeInt" class="dataRows"></td>
      <td id="ModuleSkeleton1_RuntimeFloat" class="dataRows"></td>
    </tr>    
  </table>
  <button id="ModuleSkeleton1_ResetRun" type="button" style="margin-top:3px;float:right">Reset RAM</button>
  <form><label><input type="checkbox" name="ModuleSkeleton1_SetRunBool">Auto Increment (5sec) </label><input type="submit" value="Set"></form>
  <br>
  <b>Word of the day is: </b><span id="ModuleSkeleton1_RuntimeString"></span><br>
  <form>Change the word: <input type="text" name="ModuleSkeleton1_SetRunString"  style="width: 150px;" title="Change the word of the day text"/> <input type="submit" value="Set"></form>
  <br>
  <b>Rolling average smoothing</b>
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">Rolling average Int</span></th>
      <th class="headerRows"><span style="font-weight:bold">Rolling average Float</span></th>					  			
    </tr>
    <tr>
      <td id="ModuleSkeleton1_RollingInt" class="dataRows"></td>
      <td id="ModuleSkeleton1_RollingFloat" class="dataRows"></td>
    </tr>    
  </table>				
  Add a value to Rolling Average:<br>	
  <form>Int: <input style="width: 40px;" name="ModuleSkeleton1_SetRunInt" type="number"/ value='15'/> <input type="submit" value="Set"></form>
  <form>Float: <input style="width: 50px;" name="ModuleSkeleton1_SetRunFloat" type="number"/ vstep='0.1' value='4.2'/> <input type="submit" value="Set"></form>	
  <button id="ModuleSkeleton1_ResetRollInt" type="button" style="margin-top:3px">Reset RollingInt</button>
  <button id="ModuleSkeleton1_ResetRollFloat" type="button" style="margin-top:3px">Reset RollingFloat</button>
</div>
*/