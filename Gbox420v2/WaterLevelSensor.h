#pragma once

#include "420Common.h"

class GrowBox;  //forward declaration

class WaterLevelSensor : public Common
{
  public:
    WaterLevelSensor(const __FlashStringHelper * Name, GrowBox * GBox, Settings::WaterLevelSensorSettings * DefaultSettings);  //constructor
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    void refresh_Minute();
    void report();
    int8_t getLevel();
    char * getLevelText();
    char * getLevelGauge(); 

  private:
    GrowBox * GBox; //Pointer to the GrowBox object that contains the Lights object
    bool isAboveSensor1; //Lowest water sensor, true if level reached. Below this level the reservoir should be refilled
    bool isAboveSensor2;
    bool isAboveSensor3;
    bool isAboveSensor4; //Highest water sensor. When level is reached the reservoir is full.
    byte * Pin_1;  //Lowest water level
    byte * Pin_2;
    byte * Pin_3;
    byte * Pin_4;
    char LevelGauge[9]; //Displays water level: E[____]F, E[#___]F, E[##__]F, E[###_]F, E[####]F
    int8_t Level;  //Stores water level in a number: 0 - Empty ... 4 - Full  

  protected:  
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>WaterLevel1</h1>
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">Level</span></th>	
    </tr>
    <tr>
      <td id="WaterLevel1_Level" class="dataRows"></td>
    </tr>
  </table>	
</div>
*/