#pragma once
//Defines 3 classes: Reservoir, PHSensor and WaterTempSensor

#include "420Common.h"

class GrowBox;  //forward declaration 

class PHSensor : public Common
{
  public:
    PHSensor(const __FlashStringHelper * Name, GrowBox * GBox,  Settings::PHSensorSettings * DefaultSettings);
    void websiteEvent_Load(__attribute__((unused)) char * url);
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    void websiteEvent_Button(char * Button);
    void websiteEvent_Field(char * Field);
    void refresh_Minute();
    void report();
    void updatePH(bool ShowRaw);
    void setSlope(float Value);
    void setIntercept(float Value);
    float getPH();
    char * getPHText();

  private:
    GrowBox * GBox;
    byte * Pin;
    RollingAverage * PH;
    float * Intercept;
    float * Slope;
  
  protected:    
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>PHSensor1</h1>
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">PH</span></th>			
    </tr>
    <tr>
      <td id="PHSensor1_PH" class="dataRows"></td>
    </tr>
  </table>	
</div>

<div class="card" style="width:90%">
  <h1>Calibration</h1>				
  <b>PH sensor </b><button id="PHSensor1_ReadRaw" type="button" style="margin-top:0px" title="Displays the analog PH value for a calibration point">Read raw</button>
  <form>
    Slope:<input style="width: 80px;" name="PHSensor1_Slope" type="text"/> 
    Intercept:<input style="width: 80px;" name="PHSensor1_Intercept" type="text"/>
    <input type="submit" value="Set">										
  </form>  			
</div>

*/