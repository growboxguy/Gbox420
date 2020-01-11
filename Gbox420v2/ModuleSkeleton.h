#pragma once
//This is a template for creating new modules

//#include "3rdPartyModlue.h" //Load any required 3rd party modules here
#include "420Common.h"

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
  <h1>WaterTemp1</h1>
  <table class="tg">
    <tr>				 
      <th class="headerRows"><span style="font-weight:bold">Temp</span></th>	
    </tr>
    <tr>
      <td id="WaterTemp1_Temp" class="dataRows"></td>
    </tr>
  </table>	
</div>
*/